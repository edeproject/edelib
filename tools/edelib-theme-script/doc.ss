;;
;; generic documentation system
;;

;; will be in form:
;; (list <all funcs>
;;   (vector <func name> <doc>))
(define *doc-data* '())

(define (add-doc func str)
  (set! *doc-data* (cons (vector func str) *doc-data*)) )

(add-doc "foo" "This is foo function with some cool data")
;(add-doc "baz" "Baz function has something really cool. Do you know it?")

(define (doc func)

  (define (printer v)
	(if (string=? func (vector-ref v 0))
     (display (vector-ref v 1))
	 (display "Not found") ))

  (for-each printer *doc-data*)
  (newline) )

(doc "foo")
(doc "boo")
