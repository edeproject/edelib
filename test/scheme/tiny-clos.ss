(load "test.ss")
(load "../../sslib/init-2.ss")

;; WTF? somehow *error-hook* gets redefined :S
;; TODO: inspect this
(define *error-hook* println)

;; initialize counters so we can see load time
(test-equal "starting tiny-clos..." #t #t)

(load "tiny-clos/support.scm")
(load "tiny-clos/tiny-clos.scm")

;; do some basic operations
(define <person> (make <class>
                   'direct-supers (list <object>)
                   'direct-slots (list 'name 'age)))

(define sam (make <person>))
(slot-set! sam 'age 3)
(slot-set! sam 'name "Sam John")

(test-equal "slot-ref" (slot-ref sam 'age) 3)
(test-equal "slot-ref #2" (slot-ref sam 'name) "Sam John")

(define john (make <person>))
(slot-set! john 'age 4)
(slot-set! john 'name "John Jones")

(test-equal "slot-ref #3" (slot-ref john 'age) 4)
(test-equal "slot-ref #4" (slot-ref john 'name) "John Jones")

;; constructor with simplified arguments
(add-method initialize
  (make-method (list <person>)
    (lambda (cnm obj initargs)
      (slot-set! obj 'name (car initargs))
      (unless (null? (cdr initargs))
	    (slot-set! obj 'age (cadr initargs))))))

(define john2 (make <person> "John Foo" 35))
(test-equal "slot-ref #5" (slot-ref john2 'age) 35)
(test-equal "slot-ref #6" (slot-ref john2 'name) "John Foo")

(run-all-tests "TinyCLOS Tests")
