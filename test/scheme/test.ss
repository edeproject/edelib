;;
;; Simple testing framework
;;

;; each test will be in form: '("test name" passed-or-failed)'
(define *registered-tests* '())

(define *tests-passed* 0)
(define *tests-failed* 0)

;; calculate number of digits in given number
(define (__num-digits__ n)
 (let loop ((n   n)
            (ret 1))
  (if (and
       (< n  10)
       (> n -10))
   ret
   (loop (/ n 10) (+ ret 1))
)))

;; align dots according to curr and maxnum relationship
(define (__print-dots__ curr maxnum)
 ;; start with 10 dots
 (display "..........")

 (let loop ((start (__num-digits__ curr))
            (end   (__num-digits__ maxnum)))
  (if (>= start end)
   #t
   (begin
    (display ".")
    (loop (+ 1 start) end))
)))

(define (test-print . args)
  (for-each display args))

;; check if 'expected' == 'received'
(define (test-equal name expected received)
 (cond
  ((equal? expected received)
   (set! *tests-passed* (+ 1 *tests-passed*))
   (set! *registered-tests* (cons (list name #t) *registered-tests*)))
  (else
   (set! *tests-failed* (+ 1 *tests-failed*))
   (set! *registered-tests* (cons (list name #f) *registered-tests*)))
))

(define (test-apply name expected func . args)
 (test-equal name expected (apply func args)))

;; run all tests, displaying result of each one
(define (run-all-tests title)
 (set! *registered-tests* (reverse *registered-tests*))
 (define i 1)
 (define ntests (length *registered-tests*))

 (display "* ")
 (display title)
 (display " [Passed: ")
 (display *tests-passed*)
 (display ", ")
 (display " Failed: ")
 (display *tests-failed*)
 (display "]")
 (newline)
 (display "------------------------------------------------------\n")

 (for-each (lambda (x)
             (test-print "[" i "/" ntests "]")
             (__print-dots__ i ntests)

             (if (cadr x)
              (test-print "passed   ")
              (test-print "FAILED   ")
             )

             ;; print description
             (test-print (car x) "\n")
             (set! i (+ 1 i))) 
    *registered-tests*)
 (test-print "\n")
)
