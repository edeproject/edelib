(display "Foo")
(newline)

(define (println a)
  (display a)
  (newline))

(define-macro (let1 a b . body)
  `(let ([,a ,b])
	 ,@body))

(define-macro (let2 a b c d . body)
  `(let* ([,a ,b]
		  [,c ,d])
	 ,@body))

(define-macro (let3 a b c d e f . body)
  `(let* ([,a ,b]
		  [,c ,d]
		  [,e ,f])
	 ,@body))

(let1 a 3 
  (println a))

(let2 a 3
	  b 4
  (println (+ a b)) )

(let3 a 1
	  b 2
	  c (+ a b)
  (println c))

(define-macro (defn func args . body)
  `(define (,func ,@args)
	 ,@body))

(defn oo [a b c]
  (println (+ a b c)))

(oo 1 2 3)
(println (string? "xx"))

(println (vector 1 2 3))

(define x 3)
(defn myprint [a]
  (println a))

(myprint x)
