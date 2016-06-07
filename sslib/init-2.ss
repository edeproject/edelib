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

;; uncomment these for different scheme implementations

;(define-macro (define-with-return . form)
;  `(define ,(car form)
;       (call/cc (lambda (return) ,@(cdr form)))))
;
;(define (defined? v)
;  #f)
;
;(define-macro (when s . body)
;  `(if ,s
;    (begin ,@body)))

;;; module system

(define-macro (module name exports . forms)
  `(begin
    (define ,name
       (apply (lambda () ,@forms (current-environment))))
    ,@(map (lambda (v)
             `(define ,v (eval (quote ,v) ,name)))
           exports)))

(define-macro (import module-name)
  `(load 
     (string-append
       (symbol->string
         (quote ,module-name)) ".ss")))

(module init-2
        (add-doc-generic
         add-doc
         add-macro-doc 
         add-var-doc
         find-doc
         doc
         add-to-include-path
         remove-from-include-path
         include
         eval-string
         first rest second
         print println
         empty?
         let1 letn
         if-not when-not if= if-not=
         defvar defun
         for while
         -> ->>
         nth
         sort-with-operator sort sort-vector
         filter range
         fold-right fold-left
         take drop split-at partition flatten
         repeatedly compose
         lazy replace-all
         infix->prefix :
         shuffle-vector! shuffle
         list-as-string vector-as-string
         doto zip juxt format
         edelib-scheme-objects)

;;; documentation system

;; each element will be in form (vector <func name> <doc> <type>))
;; where <type> can be in form "function" "variable" or "macro"
(define *doc-data* '())

;(define *edelib-scheme-init2-start* (edelib-clock))

(define (add-doc-generic func str type)
  (set! *doc-data* (cons (vector func str type) *doc-data*)))

(define (add-doc func str)
  (add-doc-generic func str "function"))

(define (add-macro-doc func str)
  (add-doc-generic func str "macro"))

(define (add-var-doc func str)
  (add-doc-generic func str "variable"))

(define-with-return (find-doc func)
  (let ([ff (cond
              [(string? func) func]
              [(symbol? func) (symbol->string func)]
              [else
                (error _"find-doc expects string or symbol as argument") ] ) ] )
    (for-each (lambda (x)
                (if (string=? ff (vector-ref x 0))
                  (return (vector-ref x 1)) ))
              *doc-data*)
    #f) )

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

;;; include facility

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

;;; core extensions

(add-doc "eval-string" "Evaluate string. The string can be any valid scheme expression as long as is provided as
single expression (e.g. as '(+ 1 2 3)' but not multiple one, e.g. '(+ 1 2 3) (+ 3 4)').")
(define (eval-string str)
  (eval
    (with-input-from-string str
      (lambda () (read)) ) ) )

(add-doc "first" "Return first element from the list. If list is empty, contrary to 'car' it will only return #f.")
(define (first lst)
  (if (or (null? lst)
          (not lst))
    #f
    (car lst)))

(add-doc "rest" "Return list without first element. Same as 'cdr' except it will return #f when list is empty.")
(define (rest lst)
  (if (or (null? lst)
          (not lst))
    #f
    (cdr lst)))

(add-doc "second" "Return second element from the list. If list is empty or have less elements, only return #f.")
(define (second lst)
  (first (rest lst)))

(add-doc "print" "Print values to stdout.")
(define (print . args)
  (for-each display args))

(add-doc "println" "Same as (print), except ends with newline.")
(define (println . args)
  (for-each display args)
  (newline))

(add-doc "empty?" "Check if given list is empty.")
(define (empty? lst)
  (= 0 (length lst)))

(add-macro-doc "let1" "Form for creating single local variable; same as 'let' but in readable form. Can be used as:
(let1 foo 3
  (println foo))")
(define-macro (let1 a b . body)
  `(let ([,a ,b])
     ,@body))

(add-macro-doc "letn" "Allow unlimited number of bindings to be created in single form, like in Clojure.
All bindings can be used in the next one, as 'let*' is used for the final construction.")
(define-macro (letn form . body)
  `(let* ,(partition 2 form)
     ,@body))

;(add-macro-doc "if-not" "Same as (if (not x)).")
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

(add-macro-doc "defun" "Creates function definition, allowing docstrings. Functions can be created like:
(defun sample-func (a b)
  \"This is docstring\"
  (+ a b))")
(define-macro (defun func args . body)
  `(if (string? ',(car body))
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
  (let ([coll (gensym)])
    `(let ([,coll ,collection])
       (cond
         [(list?   ,coll) (for-each (lambda (,var) ,@body) ,coll)]
         [(vector? ,coll) (for-each (lambda (,var) ,@body) (vector->list ,coll))]
         [(string? ,coll) (for-each (lambda (,var) ,@body) (string->list ,coll))]
         [else
          (throw "Unsupported type in 'for' loop")]))))

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

(add-macro-doc "->" "Clojure thrush form, where expression is threaded through the forms. The code:
  (-> 10 (+ 20) (- 30))
is equivalent to:
  (- (+ 10 20) 30)")
(define-macro (-> x form . body)
  (if (pair? body)
    `(-> (-> ,x ,form) ,@body)
    (if (list? form)
      `(,(car form) ,x ,@(cdr form))
      (list form x) )))

(add-macro-doc "->>" "Same as '->' except x is inserted as last item in form, and so on, like:
  (->> (range 1 10) (map inc))
is the same as:
  (map inc (range 1 10))")
(define-macro (->> x form . body)
  (if (pair? body)
    `(->> (->> ,x ,form) ,@body)
    (if (list? form)
      `(,(car form) ,@(cdr form) ,x)
      (list form x) )))

(add-doc "nth" "Returns index 'n' at given collection. Collection can be list, vector or string. In case of vector
or string, access is in constant time. For list, it is linear.")
(define (nth n collection)
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

(add-doc "sort-with-operator" "Sort given list 'lst' using insertion sort. 'op' is operator used for comparison.")
(define (sort-with-operator lst op)
  (define (insert n lst)
    (cond
      [(empty? lst) (cons n lst)]
      [else
        (if (op n (car lst))
            (cons n lst)
            (cons (car lst)
                  (insert n (cdr lst))) ) ] ) )

  (if (empty? lst)
    '()
    (insert (car lst) (sort-with-operator (cdr lst) op)) ) )

(add-doc "sort" "Sort list using < operator.")
(define (sort lst)
  (sort-with-operator lst <))

(add-doc "sort-vector" "Sort vector.")
(define (sort-vector v)
  (->> v vector->list sort list->vector))

(add-doc "filter" "Filter sequence with given predicate.")
(define (filter pred seq)
  (cond
    [(null? seq) '()]
    [(pred (car seq))
     (cons (car seq)
           (filter pred (cdr seq)) )]
    [else
      (filter pred (cdr seq)) ]))

(add-doc "range" "Create range starting from s and ending with e - 1.")
(define (range s e)
  (let loop ([s s]
             [e e])
    (if (>= s e)
      (list)
      (cons s (loop (+ 1 s) e)) )))

(add-doc "fold-right" "Implementation of scheme's foldr function. Tinyscheme already provides foldr which is more like foldl.")
(define (fold-right f x lst)
  (if (null? lst)
    x
    (f (car lst) (fold-right f x (cdr lst))) ))

(add-doc "fold-left" "Implementation of scheme's foldl.")
(define (fold-left f x lst)
  (if (null? lst)
    x
    (fold-left f (f x (car lst)) (cdr lst)) ))

(add-doc "take" "Take n elements from given list.")
(define (take n lst)
  (if (or (equal? lst '())
          (<= n 0))
    (list)
    (cons (car lst)
          (take (- n 1) (cdr lst)) ) ) )

(add-doc "drop" "Return new list without first n elements.")
(define (drop n lst)
  (if (or (equal? lst '())
          (<= n 0))
    lst
    (drop (- n 1) (cdr lst)) ) ) 

(add-doc "split-at" "Split given list on two sublists from position n.")
(define (split-at n lst)
  (list (take n lst)
        (drop n lst)))

(add-doc "partition" "Partition list on sublists where each sublist have n items.")
(define (partition n lst)
  (let* ([s (take n lst)]
         [l (length s)])
    (if (= n l)
      (cons s (partition n (drop n lst)))
      (list) ) ) )

(add-doc "flatten" "Return flat list of all nested sublists.")
(define (flatten lst)
  (if (list? lst)
    (cond
      [(null? lst) lst]

      [(list? (car lst))
        (append (flatten (car lst))
                (flatten (cdr lst)) )]
      [else
        (cons (car lst) (flatten (cdr lst))) ])))

(add-doc "repeatedly" "Call func n times and return the list of collected results.")
(define (repeatedly n func)
  (let loop ()
    (if (<= n 0)
      (list)
      (cons (func) (repeatedly (- n 1) func)) ) ) )

(add-doc "compose" "Returns a function which is composition of functions f and g.")
(define (compose f g)
  (lambda args
    (f (apply g args))))

(add-macro-doc "lazy" "Return lazy function with value caching. Calling that function (without parameters) will
realize sequence caching return value.")
(define-macro (lazy . body)
  (let ([forced (gensym)]
        [value  (gensym)])
    `(let ([,forced #f]
           [,value  #f])
       (lambda ()
         (unless ,forced
           (set! ,value (begin ,@body))
           (set! ,forced #t))
         ,value ))))

(add-doc "replace-all" "Replace all occurences of 'what' into 'to' in given list. Returns new list with replaced values.")
(define (replace-all what to lst)
  (if (null? lst)
    '()
    (cons (if (equal? what (car lst))
            to
            (car lst))
          (replace-all what to (cdr lst)) ))) 

;;; infix syntax

(define *edelib-scheme-precedence-table*
  '((|| 10)
    (&& 20)
    (== 30)
    (=  30)
    (!= 30)
    (<  40)
    (>  40)
    (<= 40)
    (>= 40)
    (-  50)
    (+  50)
    (/  60)
    (*  70)
    (mod 80)))

;;; latest entry in *edelib-scheme-precedence-table*
(define *edelib-scheme-precedence-table-max* 80)

(define (find-lowest-precedence lst)
  (let loop ([i           0]
             [lst         lst]
             [lowest-i    #f]
             [lowest-prec *edelib-scheme-precedence-table-max*])

    (if (= 0 (length lst))
      lowest-i
      (let* ([key  (first lst)]
             [val  (assq key *edelib-scheme-precedence-table*)]
             [prec (if val (cadr val) #f)])
        (if (and prec
                 (<= prec lowest-prec))
          (loop (+ i 1) (rest lst) i prec)
          (loop (+ i 1) (rest lst) lowest-i lowest-prec) ) ) ) ) )

(define (infix->prefix lst)
  (cond
    [(not (list? lst))  lst]
    [(= 1 (length lst)) (->> lst first infix->prefix)]
    [else
      (let1 lowest (find-lowest-precedence lst)
        (if lowest
          (let* ([lsp (split-at lowest lst)]
                 [hd  (car  lsp)]
                 [tl  (cadr lsp)]
                 [op  (car tl)]
                 [tl  (cdr tl)])
            (list op
                  (infix->prefix hd)
                  (infix->prefix tl) ) )
          lst ) ) ] ) )

(add-macro-doc ":" "Infix syntax support, e.g. '(: 2 + 3 + 100 * (20 / 3))' with variable support.")
(define-macro (: . body)
  (infix->prefix body))

;;; Fisher-Yates shuffle using 'random-next' from init.ss
(define (shuffle-vector! vec)
  (let1 len (vector-length vec)
    (let loop ([i 0]
               [j (modulo (random-next) len)])
      (when (< i len)
        (let ([tmp1 (vector-ref vec i)]
              [tmp2 (vector-ref vec j)])
          (vector-set! vec j tmp1)
          (vector-set! vec i tmp2)

          (loop (succ i)
                (modulo (random-next) len) ) ) ) ) 
    vec) )

(add-doc "shuffle" "Return new shuffled list. Note how this function uses 'random-next' where
initial state will likely be the same every time, so either call (random-next) unspecified
number of times before, or call (shuffle lst) different times within each call.")
(define (shuffle lst)
  (->> lst
       list->vector
       shuffle-vector!
       vector->list))

;;; to string conversions

(add-doc "list-as-string" "Convert list to string.")
(define (list-as-string lst)
  (let ([len (length lst)]
        [ret "("])
      
    (define (str-append! s add-space)
      (set! ret (string-append ret s))
      (if add-space
        (set! ret (string-append ret " "))))

    (define (str-trim-right!)
      (let ([len (string-length ret)])
        (if (>= len 2)
          (set! ret (substring ret 0 (- len 1))))))

    (let loop ([lst lst])
      (for-each (lambda (x)
                  (cond
                    [(list? x)
                     (str-append! "(" #f)
                     (loop x)
                     (str-append! ") " #f)]

                    [(vector? x)
                     (str-append! "#(" #f)
                     (loop (vector->list x))
                     (str-append! ") " #f)]

                    ;; hm... bug probably, as (atom? vector) => #t
                    [(atom? x) (str-append! (atom->string x) #t)]
                    [else
                      (error "Unknown type in 'list->string'. Got:" x) ] ) )
                lst))

    ;; close everything
    (str-trim-right!)
    (str-append! ")" #f)
    ret
) )

(add-doc "vector-as-string" "Convert vector to string.")
(define (vector-as-string vec)
  (string-append "#" (->> (vector->list vec) list-as-string)))

(define-macro (doto x . forms)
  (let ([gx (gensym)])
    `(let ([,gx ,x])
       ,@(map (lambda (form)
                (if (list? form)
                  `(,(first form) ,gx ,@(rest form))
                  `(,form ,gx)))
              forms)
       ,gx)))

(add-doc "zip" "Zip lists, e.g. (zip '(1 2 3) '(a b c)) => '((1 a) (2 b) (3 c))")
(define (zip list1 . lists)
  (apply map list list1 lists))

(add-doc "juxt" "Takes a set of functions and returns a juxtapoisition of those
functions. Example: ((juxt + -) 2 3) => ((+ 2 3) (- 2 3)) => (5 -1).")
(define (juxt . funcs)
  (lambda args
    (map
      (lambda (f)
        (apply f args))
      funcs)))

(add-doc "format" "Allow string formatting. Implements SRFI-28.")
(define (format format-string . args)
  (let ([buffer (open-output-string)])
    (let loop ([format-list (string->list format-string)]
               [objects     args])
      (cond
        [(null? format-list)
         (get-output-string buffer)]
        [(char=? (car format-list) #\~)
         (if (null? (cdr format-list))
           (error 'format _"Incomplete escape sequence")
           (case (cadr format-list)
             [(#\a #\A)
              (if (null? objects)
                (error 'format _"No value to escape sequence")
                (begin
                  (display (car objects) buffer)
                  (loop (cddr format-list) (cdr objects))))]
             [(#\s #\S)
              (if (null? objects)
                (error 'format _"No value for escape sequence")
                (begin
                  (write (car objects) buffer)
                  (loop (cddr format-list) (cdr objects))))]
             [(#\%)
              (newline buffer)
              (loop (cddr format-list) objects)]
             [(#\~)
              (display "~" buffer)
              (loop (cddr format-list) objects)]
             [else
              (error 'format _"Unrecognized escape sequence")]))]
        [else
          (write-char (car format-list) buffer)
          (loop (cdr format-list) objects)]))))

;;; interpreter specific stuff

(add-doc "edelib-scheme-objects" "Return list of currently existing objects inside interpreter.")
(define (edelib-scheme-objects)
  (map car (oblist)))

;(println "Started in: " (edelib-clock-diff (edelib-clock) *edelib-scheme-init2-start*))

) ;; module
