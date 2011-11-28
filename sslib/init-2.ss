;; vim:ft=scheme:expandtab
;; $Id: String.h 2839 2009-09-28 11:36:20Z karijes $
;;
;; Copyright (c) 2005-2011 edelib authors
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

;(include "doc.ss")
(load "doc.ss")

(add-doc "print" "Print values to stdout.")
(define (print . args)
  (for-each display args))

(add-doc "println" "Same as (print), except ends with newline.")
(define (println . args)
  (for-each display args)
  (newline))

(add-macro-doc ":" "Simple infix operator. (: 2 + 3) will be transformed to (+ 2 3).")
(define-macro (: a op b)
  `(,op ,a ,b))

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

(add-macro-doc "defn" "Creates function definition, allowing docstrings. Function can be created like:
(defn sample-func [a b]
  \"This is docstring\"
  (+ a b))")
(define-macro (defn func args . body)
  `(if (string? ,(car body))
     ;; with docstring
     (begin
       (add-doc (symbol->string ',func) ,(car body))
       (define (,func ,@args)
         ,@(cdr body) ))
     ;; without docstring
     (define (,func ,@args)
       ,@body) ))

(add-macro-doc "defvar" "Creates variable with value, allowing docstring. Variables can be created like:
(defvar foo 3
  \"This is docstring for foo.\")")
(define-macro (defvar var val . body)
  `(begin
     ;; here we are not using ,(car body) as this would evaluate expression first, yielding
     ;; car to fail on empty list; instead we convert whatever to symbol, then evaluate
     (if (= 1 (length ',body))
       (add-var-doc (symbol->string ',var) (car ',body)) )
     (define ,var ,val) 
) )

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

(defun nth (n collection)
  "Returns index 'n' at given collection. Collection can be list, vector or string."
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
    (insert (car lst) (sort (cdr lst))) 
) )

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
	  (filter pred (cdr seq)) ]
) )

(defvar *foo* 3 "SSSSS")
(defvar *boo* 3)
;(defvar *moo* 3)
;
(println (doc "*foo*"))
(println *foo*)
(println (doc "*boo*"))
(println (doc "if="))
(println (doc "filter"))