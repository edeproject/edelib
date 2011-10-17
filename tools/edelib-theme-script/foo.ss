(include "doc.ss")

(display "Foo")
(newline)

(define (print . args)
  (for-each display args))

(define (println . args)
  (for-each display args)
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

(define-macro (let4 a b c d e f g h . body)
  `(let* ([,a ,b]
		  [,c ,d]
		  [,e ,f]
		  [,g ,h])
	 ,@body))

(define-macro (let5 a b c d e f g h i j . body)
  `(let* ([,a ,b]
		  [,c ,d]
		  [,e ,f]
		  [,g ,h]
		  [,i ,j])
	 ,@body))

(define-macro (let6 a b c d e f g h i j k l . body)
  `(let* ([,a ,b]
		  [,c ,d]
		  [,e ,f]
		  [,g ,h]
		  [,i ,j]
		  [,k ,l])
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

(let4 a 1
	  b (+ 1 a)
	  c b
	  d a
  (println "==> " (+ a b c d)) )

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

(define-macro (if= arg1 arg2 . body)
  `(if (equal? ,arg1 ,arg2)
	 ,@body))

(define-macro (if-not= arg1 arg2 . body)
  `(if (not (equal? ,arg1 ,arg2))
	 ,@body))

(if= 3 4
  (println "equal")
  (println "not equal"))

(if-not= 4 3
  (println "not equal"))

(println "XXXXXXXXXXXXXXXXXXXXXXX")
(define-macro (defn2 func args . body)
  `(if (string? ,(car body))
	 ;; with docstring
	 (begin
	   (add-doc (symbol->string ',func) ,(car body))
	   (define (,func ,@args)
	     ,@(cdr body) ))
	 ;; without docstring
	 (define (,func ,@args)
	   ,@body) ))

(defn2 foo [a b]
  "This is some doc"
  (+ a b))

;(println (doc "foob"))
(println (foo 1 2))

;; translation
(println _"This is some text")

;(println (-> "XXX" (println)))

(defn foo [args]
  (println args))

(foo 1 2 3 4 5 6)

(define (join tok . args)
  (let1 ret ""
	(define (appender s)
	  (set! ret (string-append ret tok s)))
    (for-each appender args)
	ret ))

(println (join "/" "foo" "boo" "moo" "goo"))

(define (theme.mystyle name . args)
  (println "---------> " name)
  (println args))

(theme.mystyle "ede" [
  :scheme "ede"
  :style  "gtk+"
  :foreground "#ffff"
  :background "#eeee"
])

;(println (foldr cons '() '(1 2 3 4 5) '(a b c d e f)))

(println (list->vector '()))
(println "-------------->8----------------------")
(define (-> . args)
  (println args))


(println "XXX" (sqrt 4))
