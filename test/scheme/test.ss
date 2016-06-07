;; simple testing framework for scheme inside edelib
;; keep things less sslib dependable as possible since it will test sslib itself

;; each test will be in form: '("test name" passed-or-failed)'
(define *registered-tests* '())

(define *tests-passed* 0)
(define *tests-failed* 0)
(define *tests-starting-time* 0)

(define (test-print . args)
  (for-each display args))

;; check if 'expected' == 'received'
(define (test-equal name expected received)
  (if (= *tests-starting-time* 0)
    (set! *tests-starting-time* (edelib-clock)))

  (cond
    ((equal? expected received)
	 (set! *tests-passed* (+ 1 *tests-passed*))
	 (set! *registered-tests* (cons (list name #t) *registered-tests*)))
    (else
	 (set! *tests-failed* (+ 1 *tests-failed*))
	 (set! *registered-tests* (cons (list name #f) *registered-tests*))
) ) )

(define (test-apply name expected func . args)
  (test-equal name expected (apply func args)))

(define (run-all-tests title)
  (define *tests-ending-time* (edelib-clock))
  (define i 1)
  (define ntests (length *registered-tests*))
  (set! *registered-tests* (reverse *registered-tests*))

  (test-print "* " title "\n")

  (define (printer item)
  	(test-print "Test " i ": " (car item))
	;; emit secret marker so run.sh can append needed number of spaces
	;; A hack, I know :S ...
	(test-print "=!=")
  
  	(if (cadr item)
  	  (test-print "OK")
  	  (test-print "FAILED"))
  
  	(test-print "\n")
  	(set! i (+ 1 i))
  )

  (for-each printer *registered-tests*)
  (test-print "\nTotal tests: " ntests " executed in " (edelib-clock-diff *tests-ending-time* *tests-starting-time*) " sec.\n")
  (test-print "Passed: " *tests-passed* "\n")
  (test-print "Failed: " *tests-failed* "\n\n")
)
