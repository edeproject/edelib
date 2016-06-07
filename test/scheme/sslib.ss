(load "test.ss")

(test-equal "infix" (: 2 + (3 - 4)) 1)
(test-equal "infix #2" (: 1000 - 1000) 0)

(test-equal "let1" (let1 a 3
                     (+ a 1))
                   4)

(test-equal "letn" (letn [a 1
                          b (+ a 1)
                          c 1
                          d (+ a b c)
                          e a]
                     (+ a b c d e))
                   9)

(test-equal "if=" 0
                  (if= 3 3
                    0
                    1))

(test-equal "if= #2" 0
                     (if= '(1 2 3) '(1 2 3)
                       0
                       1))
            
(test-equal "if= #3" 1
                     (if= "this is some string" "this is another string"
                       0
                       1))

(test-equal "if-not=" 0
                      (if-not= '(1 2 3) '()
                        0
                        1))

(test-equal "if-not= #2" 1
                         (if-not= #(1 2 3) #(1 2 3)
                           0
                           1))

(defvar some-demo-var 3)
(test-equal "defvar" some-demo-var 3)

(defvar some-demo-var 4)
(test-equal "defvar #2" some-demo-var 4)

(defun some-func ()
  3)
(test-equal "defun" (some-func) 3)

(defun some-func2 ()
  "We have some string"
  4)
(test-equal "defun #2" (some-func2) 4)

(test-equal "nth" 100 (nth 2 '(0 300 100 2 3 4 5)))
(test-equal "nth #2" #\s (nth 3 "This is some test"))
(test-equal "nth #3" 1   (nth 0 #(1 2 3 4 5 6)))
(test-equal "nth #4" #f  (nth 100 '()))

(test-equal "sort" '(1 2 3 4 5) (sort '(4 2 1 3 5)))

(test-equal "sort-vector" #(1 2 3 4 5) (sort-vector #(4 2 1 5 3)))

(test-equal "range" '(1 2 3 4 5 6 7) (range 1 8))

(test-equal "->" (-> 10 (+ 20) (- 30) (+ 40)) 40)
(test-equal "-> #2" (-> 2 (nth '(4 2 100 34 3))) 100)

(test-equal "->>" (->> '(1 2 3)
                       (map (lambda (x)
                              (+ x 1) )))
                  '(2 3 4))

(test-equal "take" (take 2 '(1 2 3))
                   '(1 2))

(test-equal "take #2" (take 100 '(1 2))
                      '(1 2))

(test-equal "take #3" (take 1000 '()) '())

(test-equal "drop" (drop 1 '(1 2 3 4)) '(2 3 4))
(test-equal "drop #2" (drop 1000 '()) '())
(test-equal "drop #3" (drop 3 '(1 2 3)) '())

(test-equal "partition" (partition 2 '(1 2 3 4)) '((1 2) (3 4)))
(test-equal "partition #2" (partition 100 '(1 2 3)) '())
(test-equal "partition #3" (partition 1 '(1 2 3))   '((1) (2) (3)))

(test-equal "flatten" (flatten '(1 2 3 (4 5) (((((6))))))) '(1 2 3 4 5 6))
(test-equal "flatten #2" (flatten '()) '())
(test-equal "flatten #3" (flatten #f) '())
(test-equal "flatten #4" (flatten '(1 2 3 4 5)) '(1 2 3 4 5))

(test-equal "replace-all" (replace-all 3 100 '(1 2 3 3 4 3)) '(1 2 100 100 4 100))
(test-equal "replace-all #2" (replace-all 1 100 '(2 3 4 5 6)) '(2 3 4 5 6))
(test-equal "replace-all #3" (replace-all '<> "foo" '(2 3 <> "boo" <> <> "moo")) '(2 3 "foo" "boo" "foo" "foo" "moo"))

;; integer->char fixes
(test-equal "integer->char fix" (integer->char 256) #\nul)

;; tinyscheme will on (make-vector) produce #(() ()...) but guile #(<unspecified>...) so we must init all
;; fields so comparison can work
(test-equal "doto" (doto (make-vector 3)
                     (vector-set! 0 1)
                     (vector-set! 1 1)
                     (vector-set! 2 1))
                   #(1 1 1))

(test-equal "repeatedly" (repeatedly 4 [lambda () (range 1 4)])
                         '((1 2 3) (1 2 3) (1 2 3) (1 2 3)))

(test-equal "repeatedly #2" (repeatedly 10 (lambda () 1))
                            '(1 1 1 1 1 1 1 1 1 1))

(test-equal "zip" (zip '(1 2 3) '(a b c) '(d e f))
                  '((1 a d) (2 b e) (3 c f)))

(test-equal "juxt" ((juxt max min) 14 35 -7 46 98)
                   '(98 -7))			

(test-equal "juxt #2" ((juxt + - *) 2 3)
                      '(5 -1 6))			

(test-equal "juxt #3" ((juxt string-length string->list) "demo")
                      '(4 (#\d #\e #\m #\o)))	

(test-equal "format #1" "This is sample string!" (format "This is sample string!"))
(test-equal "format #2" "This is sample string!" (format "This is ~A string!" "sample"))
(test-equal "format #3" "This is sample string!" (format "This is ~a string!" "sample"))
(test-equal "format #4" "This is \"sample\" string!" (format "This is ~s string!" "sample"))
(test-equal "format #5" "This is \"sample\" string!" (format "This is ~S string!" "sample"))
(test-equal "format #6" "range: (1 2 3 4 5)" (format "range: ~a" (range 1 6)))
(test-equal "format #7" "this string have vector: #(1 2 3) list: (1 2 3) string: \"xxx\" and newline\n"
						(format "this ~A have vector: ~A list: ~A string: ~S and newline~%" "string" #(1 2 3) '(1 2 3) "xxx"))
(test-equal "format #8" "this is ~" (format "this is ~~"))
(test-equal "format #9" "this is ~~" (format "this is ~~~~" 123))
(test-equal "format #10" "this is \n3" (format "this is ~%~A" 3))

(run-all-tests "sslib Tests")
