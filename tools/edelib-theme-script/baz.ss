;; stream example

(define (println . args)
  (for-each display args)
  (newline))

(define (filter pred seq)
  (cond
	((null? seq) '())
	((pred (car seq))
	 (cons (car seq)
		   (filter pred (cdr seq)) ) )
	(else
	  (filter pred (cdr seq)) )
) )

(define (accumulate op initial seq)
  (if (null? seq)
	initial
	(op (car seq)
		(accumulate op initial (cdr seq))
) ) )

(define (enumerate-interval low high)
  (if (> low high)
	'()
	(cons low (enumerate-interval (+ low 1) high))
) )

(println (enumerate-interval 2 7))
