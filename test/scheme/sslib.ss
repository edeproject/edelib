(load "test.ss")
(load "../../sslib/init-2.ss")

(test-equal "#1" (: 2 + (3 - 4)) 1)
(test-equal "#1" (: 1000 - 1000) 0)

(test-equal "#2" (let1 a 3
				   (+ a 1))
			     4)

(test-equal "#3" (let2 a 3
					   b 4
				   (+ a b))
			     7)

(test-equal "#4" (let3 a 1
					   b 3
					   c 100
				   (* a b c))
			     300)

(test-equal "#5" (let4 a 1
					   b a
					   c b
					   d c
				   (+ a b c d))
			     4)

(test-equal "#6" (let5 a 1
					   b (+ a 1)
					   c 1
					   d (+ a b c)
					   e a
				   (+ a b c d e))
			     9)

(test-equal "#7" 0
			     (if= 3 3
				   0
				   1))

(test-equal "#7" 0
			     (if= '(1 2 3) '(1 2 3)
				   0
				   1))
			
(test-equal "#7" 1
			     (if= "this is some string" "this is another string"
				   0
				   1))

(test-equal "#8" 0
			     (if-not= '(1 2 3) '()
				   0
				   1))

(test-equal "#8" 1
			     (if-not= #(1 2 3) #(1 2 3)
				   0
				   1))

(defvar some-demo-var 3)
(test-equal "#9" some-demo-var 3)

(defvar some-demo-var 4)
(test-equal "#9" some-demo-var 4)

(defun some-func ()
  3)
(test-equal "#10" (some-func) 3)

(defun some-func2 ()
  "We have some string"
  4)
(test-equal "#10" (some-func2) 4)

(test-equal "#11" 100 (nth 2 '(0 300 100 2 3 4 5)))
(test-equal "#11" #\s (nth 3 "This is some test"))
(test-equal "#11" 1   (nth 0 #(1 2 3 4 5 6)))
(test-equal "#11" #f  (nth 100 '()))

(test-equal "#12" '(1 2 3 4 5) (sort '(4 2 1 3 5)))

(test-equal "#13" #(1 2 3 4 5) (sort-vector #(4 2 1 5 3)))

(test-equal "#14" '(1 2 3 4 5 6 7) (range 1 8))

(test-equal "#15" (-> 10 (+ 20) (- 30) (+ 40)) 40)
(test-equal "#15" (-> 2 (nth '(4 2 100 34 3))) 100)

(test-equal "#16" (->> '(1 2 3)
                       (map (lambda (x)
                              (+ x 1) )))
                  '(2 3 4))

(test-equal "#17" (take 2 '(1 2 3))
			      '(1 2))

(test-equal "#17" (take 100 '(1 2))
			      '(1 2))

(test-equal "#17" (take 1000 '()) '())

(test-equal "#18" (drop 1 '(1 2 3 4)) '(2 3 4))
(test-equal "#18" (drop 1000 '()) '())
(test-equal "#18" (drop 3 '(1 2 3)) '())

(test-equal "#19" (partition 2 '(1 2 3 4)) '((1 2) (3 4)))
(test-equal "#19" (partition 100 '(1 2 3)) '())
(test-equal "#19" (partition 1 '(1 2 3))   '((1) (2) (3)))

(test-equal "#20" (flatten '(1 2 3 (4 5) (((((6))))))) '(1 2 3 4 5 6))
(test-equal "#20" (flatten '()) '())
(test-equal "#20" (flatten #f) '())
(test-equal "#20" (flatten '(1 2 3 4 5)) '(1 2 3 4 5))

(test-equal "#21" (replace-all 3 100 '(1 2 3 3 4 3)) '(1 2 100 100 4 100))
(test-equal "#21" (replace-all 1 100 '(2 3 4 5 6)) '(2 3 4 5 6))
(test-equal "#21" (replace-all '<> "foo" '(2 3 <> "boo" <> <> "moo")) '(2 3 "foo" "boo" "foo" "foo" "moo"))

(run-all-tests "sslib Tests")
