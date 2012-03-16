;; vim:ft=scheme:expandtab
;; $Id: String.h 2839 2009-09-28 11:36:20Z karijes $
;;
;; Copyright (c) 2005-2012 edelib authors
;;
;; This library is free software; you can redistribute it and/or
;; modify it under the terms of the GNU Lesser General Public
;; License as published by the Free Software Foundation; either
;; version 2 of the License, or (at your option) any later version.
;;
;; This library is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
;; Lesser General Public License for more details.
;;
;; You should have received a copy of the GNU Lesser General Public License
;; along with this library. If not, see <http://www.gnu.org/licenses/>.

;;
;; documentation system
;;

;; each element will be in form (vector <func name> <doc> <type>))
;; where <type> can be in form "function" "variable" or "macro"
(define *doc-data* '())

(define (add-doc-generic func str type)
  (set! *doc-data* (cons (vector func str type) *doc-data*)) )

(define (add-doc func str)
  (add-doc-generic func str "function"))

(define (add-macro-doc func str)
  (add-doc-generic func str "macro"))

(define (add-var-doc func str)
  (add-doc-generic func str "variable"))

(define-with-return (find-doc func)
  (unless (string? func) (error _"find-doc expects string argument"))

  (for-each (lambda (x)
              (if (string=? func (vector-ref x 0))
                (return (vector-ref x 1)) ))
            *doc-data*)
  #f)

(define (doc func)
  (define ret (find-doc func))
  (if ret
    (begin
      (display func)
      (display "\n----------\n")
      (display ret))
    (display _"Not found"))
  (newline))

(add-doc "find-doc" "Try to find documentation for given parameter. Return documentation string or #f if fails.")
(add-doc "doc" "Display documentation for given function name string.")
(add-doc "add-doc" "Add documentation for given functioa.n")
(add-doc "add-macro-doc" "Add documentation for macro.")
(add-doc "add-var-doc" "Add documentation for variable.")

;;
;; include facility
;;

(or (defined? '*include-path*)
    (define *include-path* (list)))

(add-doc "add-to-include-path" "Allow 'include' to find specified path.")
(define (add-to-include-path path)
  (set! *include-path*
    (cons path *include-path*)))

(add-doc "remove-from-include-path" "Remove path from include list.")
(define (remove-from-include-path path)
  (let ([n (list)])
    (for-each (lambda (p)
                (if (not (= p path))
                  (set! n (cons p n)) ))
              *include-path*)
    (set! *include-path* n) 
) )

(add-doc "include" "Load file. The file will be searched in *include-path* list. You can simply alter
this list with 'add-to-include-path' and 'remove-from-include-path' functions.")
(define-with-return (include object)
  ;; simple function for checking file existance; could be, in some future, replaced
  ;; with builtin one
  (define (file-exists f)
    (let* ([fd   (open-input-file f)]
           [ret  (if fd #t #f)])
      (if fd
        (close-input-port fd))
      ret))

  (for-each (lambda (x)
              (define full-path (string-append x "/" object))
              (when (file-exists full-path)
                    (load full-path)
                    (return #t) ))
            *include-path*
) )

;;
;; core extensions
;;

(add-doc "print" "Print values to stdout.")
(define (print . args)
  (for-each display args))

(add-doc "println" "Same as (print), except ends with newline.")
(define (println . args)
  (for-each display args)
  (newline))

(define (infix->prefix-func ex)
  ;; simple check to see if we have binary operation
  (define (binary-exp? x)
    (and (list? x)
         (= 3 (length x))))

  ;; convert everything to prefix, and evaluate
  (eval 
    (if (atom? ex)
      ex
      (map infix->prefix-func
           (if (binary-exp? ex)
             (list
               (cadr ex)    ;; operator
               (car ex)     ;; first elem.
               (caddr ex))  ;; second elem.
             ex )))
) )

(define-macro (infix->prefix . args)
  `(infix->prefix-func ',args))

(add-macro-doc ":" "Infix operator. Operator precedence is not supported, so you must use parenthesis. Examples:
(: 2 + 3)
(: 2 + (3 - 100))
(: 2 * (3 - (100 + 4)))")
(define-macro (: . args)
  `(infix->prefix-func ',args))

(add-macro-doc "let1" "Form for creating single local variable; same as 'let' but in readable form. Can be used as:
(let1 foo 3
  (println foo))")
(define-macro (let1 a b . body)
  `(let ([,a ,b])
     ,@body))

(add-macro-doc "let2" "Same as 'let1' but creates two variables.")
(define-macro (let2 a b c d . body)
  `(let* ([,a ,b]
          [,c ,d])
     ,@body))

(add-macro-doc "let3" "Same as 'let1' but creates three variables.")
(define-macro (let3 a b c d e f . body)
  `(let* ([,a ,b]
          [,c ,d]
          [,e ,f])
     ,@body))

(add-macro-doc "let4" "Same as 'let1' but creates four variables.")
(define-macro (let4 a b c d e f g h . body)
  `(let* ([,a ,b]
          [,c ,d]
          [,e ,f]
          [,g ,h])
     ,@body))

(add-macro-doc "let5" "Same as 'let1' but creates five variables.")
(define-macro (let5 a b c d e f g h i j . body)
  `(let* ([,a ,b]
          [,c ,d]
          [,e ,f]
          [,g ,h]
          [,i ,j])
     ,@body))

(add-macro-doc "if-not" "Same as (if (not x)).")
(define-macro (if-not p . body)
  `(if (not ,p)
     ,@body))

(add-macro-doc "when-not" "Same as (when (not x)).")
(define-macro (when-not p . body)
  `(when (not ,p)
     ,@body))

(add-macro-doc "if=" "Compare two variables using equal?. Can be used as:
(if= a b
  do if are equal
  do if are not equal)")
(define-macro (if= arg1 arg2 . body)
  `(if (equal? ,arg1 ,arg2)
     ,@body))

(add-macro-doc "if-not=" "Same as 'if=' but opposite.")
(define-macro (if-not= arg1 arg2 . body)
  `(if (not (equal? ,arg1 ,arg2))
     ,@body))

(add-macro-doc "defvar" "Creates variable with value, allowing docstring. Variables can be created like:
(defvar foo 3 \"This is docstring for foo.\")")
(define-macro (defvar var val . body)
  `(begin
     ;; here we are not using ,(car body) as this would evaluate expression first, yielding
     ;; car to fail on empty list; instead we convert whatever to symbol, then evaluate
     (if (= 1 (length ',body))
       (add-var-doc (symbol->string ',var) (car ',body)) )
     (define ,var ,val) ))

(add-macro-doc "defun" "Creates function definition, allowing docstrings. Function can be created like:
(defun sample-func (a b)
  \"This is docstring\"
  (+ a b))")
(define-macro (defun func args . body)
  `(if (string? ,(car body))
     ;; with docstring
     (begin
       (add-doc (symbol->string ',func) ,(car body))
       (define (,func ,@args)
         ,@(cdr body) ))
     ;; without docstrings
     (define (,func ,@args)
       ,@body )))

(add-macro-doc "for" "Loop construct that can be used over vectors, lists and strings. Looping is done as:
  (for i in '(1 2 3 4 5)
    (println i))
or
  (for i in \"this is sample string\"
    (println i))")     
(define-macro (for var _ collection . body)
  `(cond
     [(list? ,collection)
      (for-each (lambda (,var) 
             ,@body) 
           ,collection)]
     [(vector? ,collection)
      (for-each (lambda (,var)
             ,@body)
           (vector->list ,collection) )]
     [(string? ,collection)
      (for-each (lambda (,var)
             ,@body)
           (string->list ,collection) )]
     [else
       (throw "Unsupported type in 'for' loop") ]))

(add-macro-doc "while" "Looping construct found in common languages. Example:
  (define i 0)
  (while (< i 100)
    (println i)
    (set! i (+ 1 i)))")
(define-macro (while expr . body)
  `(let loop ()
     (when ,expr
       ,@body
       (loop) )))

(add-macro-doc "->" "Clojure thrush form, where expression is threaded
through the forms. The code:
  (-> 10 (+ 20) (- 30))
is equivalent to:
  (- (+ 10 20) 30)")
(define-macro (-> x form . body)
  (if (pair? body)
    `(-> (-> ,x ,form) ,@body)
	(if (list? form)
	  `(,(car form) ,x ,@(cdr form))
      (list form x) )))

(add-macro-doc "->>" "Same as '->' except x is inserted as last item in
form, and so on, like:
  (->> (range 1 10) (map inc))
is the same as:
  (map inc (range 1 10))")
(define-macro (->> x form . body)
  (if (pair? body)
    `(->> (->> ,x ,form) ,@body)
	(if (list? form)
	  `(,(car form) ,@(cdr form) ,x)
	  (list form x) )))

(defun nth (n collection)
  "Returns index 'n' at given collection. Collection can be list, vector or string. In case of vector
or string, access is in constant time. For list, it is linear."
  (cond
    [(list? collection)
     (if (>= n (length collection))
       #f
       (list-ref collection n) )]
    [(vector? collection)
     (if (>= n (vector-length collection))
       #f
       (vector-ref collection n) )]
    [(string? collection)
     (if (>= n (string-length collection))
       #f
       (string-ref collection n) )]
    [else
      (error _"Unknown collection type") ]))

(defun sort-with-operators (lst op1 op2)
  "Sort given list 'lst' using insertion sort. 'op1' and 'op2' are operators
used for comparison."
  (define (empty? lst)
    (= 0 (length lst)))

  (define (insert n lst)
    (cond
      [(empty? lst) (cons n lst)]
      [else
        (cond
          [(op1 n (car lst)) (cons n lst)]
          [(op2 n (car lst)) (cons (car lst) (insert n (cdr lst)))] ) ] ) )

  (if (empty? lst)
    '()
    (insert (car lst) (sort (cdr lst))) ))

(defun sort (lst)
  "Sort list."
  (sort-with-operators lst <= >))

(defun sort-vector (v)
  "Sort vector."
  (list->vector (sort (vector->list v))))

(defun filter (pred seq)
  "Filter sequence with given predicate."
  (cond
    [(null? seq) '()]
    [(pred (car seq))
     (cons (car seq)
           (filter pred (cdr seq)) )]
    [else
      (filter pred (cdr seq)) ]))

(defun range (s e)
  "Create range starting from s and ending with e - 1."
  (let loop ([s s]
             [e e])
    (if (>= s e)
      (list)
      (cons s (loop (+ 1 s) e)) )))

(defun fold-right (f x lst)
  "Implementation of scheme's foldr function. Tinyscheme already
provides foldr which is more like foldl."
  (if (null? lst)
    x
	(f (car lst) (fold-right f x (cdr lst))) ))

(defun fold-left (f x lst)
  "Implementation of scheme's foldl."
  (if (null? lst)
    x
	(fold-left f (f x (car lst)) (cdr lst)) ))

(defun take (n lst)
  "Take n elements from given list."
  (when (> n 0)
    (if-not= lst '()
      (cons (car lst)
            (take (- n 1) (cdr lst)) ))))

(defun drop (n lst)
  "Return new list without first n elements."
  (let1 len (length lst)
    (if (> n len)
      lst
      (let loop ([n   n]
                 [lst lst])
        (if (> n 0)
          (loop (- n 1) (cdr lst))
          lst )))))

(defun partition (n lst)
  "Partition list on sublists where each sublist have n items."
  (if (> n 0)
    (let1 s (take n lst)
	  (if (= n (length s))
	    (cons s (partition n (drop n lst))) ))
	(list) ))

(defun flatten (lst)
  "Return flat list of all nested sublists."
  (if (list? lst)
	(cond
	  [(null? lst) lst]

	  [(list? (car lst))
	    (append (flatten (car lst))
				(flatten (cdr lst)) )]
	  [else
		(cons (car lst) (flatten (cdr lst))) ])))

;;
;; interpreter specific stuff
;;

(defun edelib-scheme-objects ()
  "Return list of currently existing objects inside interpreter."
  (map car (oblist)))
