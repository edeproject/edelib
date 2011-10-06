;;
;; generic documentation system
;;

;; will be in form:
;; (list <all funcs>
;;   (vector <func name> <doc>))
(define *doc-data* '())

(define (add-doc func str)
  (set! *doc-data* (cons (vector func str) *doc-data*)) )

(define (doc func)

  (define (printer v)
	(if (string=? func (vector-ref v 0))
     (display (vector-ref v 1))
	 (display "Not found") ))

  (for-each printer *doc-data*)
  (newline) )
