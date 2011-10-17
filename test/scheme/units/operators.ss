
(test-equal "Test +" 1 (+ 0 1))

(test-equal "Test -" 5 (- 7 2))

(test-equal "Test *" 20 (* 5 4))

(test-equal "Test /" 54 (/ 162 3))

(test-equal "Test negate +" -75 (+ -54 -21))

(test-equal "Test negate -" -109 (- -110 -1))

(test-equal "Test negate *" -110 (* 110 -1))

(test-equal "Test negate /" 4 (/ -20 -5))

(test-equal "Test >" #t (and (> 3 2) 
                             (> -1 -100) 
                             (> 0 -100)))

(test-equal "Test <" #t (and (< 2 3)
                             (< 100 1000)
                             (< -1 0)))

(test-equal "Test <=" #t (and (<= 2 2)
                              (<= 2 3)
                              (<= -1 0)))

(test-equal "Test >=" #t (and (>= 2 2)
                              (>= 3 2)
                              (>= 0 -1)))
