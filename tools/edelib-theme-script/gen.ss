(load "foo.ss")

(println "-----------------------------------")

(define *current-klass* #f)

(define (method klass func)
  (println "static void " klass "_" func "(pointer args) {
}"
))


(define (define-class klass klass-str . members)
  (set! *current-klass* (list klass klass-str))
  (for-each
	(lambda (x)
	  (eval x))
	members))

(define-class "String" "string"
  (method "String" "new"))
