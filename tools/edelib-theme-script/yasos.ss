;; simple object system

(define (make-point the-x the-y)
  (lambda (message)
	(case message
	  [(x) (lambda () the-x)]
	  [(y) (lambda () the-y)]
	  [(set-x!)
	   (lambda (n)
		 (set! the-x n) )]
	  [(set-y!)
	   (lambda (n)
		 (set! the-y n) )]
	  [else
		(error "Unknown message:" message)
] ) ) )

(define (make-point2 the-x the-y)
  (define (get-x) the-x)
  (define (get-y) the-y)
  
  (define (set-x! new-x)
	(set! the-x new-x)
	the-x)

  (define (set-y! new-y)
	(set! the-y new-y)
	the-y)

  (define (self message)
	(case message
	  ((x) get-x)
	  ((y) get-y)
	  ((set-x!) set-x!)
	  ((set-y!) set-y!)
	  (else
		(error "Unknown message:" message) 
  ) ) )

  self)

(define p1 (make-point 123 123))
(display ((p1 'x)))
(newline)

(define p2 (make-point 123 123))
(display ((p2 'x)))
(newline)
