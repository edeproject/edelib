;;
;; Large part is stolen from chibi-scheme. 
;;
;; Checking for supported symbol characters and disjoint-type-functions are from Gambit but
;; are modified for tinyscheme testsuite.

(load "test.ss")

;; test that all symbol characters are supported
(test-equal "#1" 17 (length '(+ - ... !.. $.+ %.- &.! *.: /:. :+. <-. =. >. ?. ~. _. ^.)))

(define disjoint-type-functions
  (list boolean? char? null? number? pair? procedure? string? symbol? vector?))

(define type-examples
  (list
   #t #f #\a '() 9739 '(test) "test" "" 'test '#() '#(a b c) ))

(test-equal "#2" 
 '((#t #f #f #f #f #f #f #f #f)
   (#t #f #f #f #f #f #f #f #f)
   (#f #t #f #f #f #f #f #f #f)
   (#f #f #t #f #f #f #f #f #f)
   (#f #f #f #t #f #f #f #f #f)
   (#f #f #f #f #t #f #f #f #f)
   (#f #f #f #f #f #f #t #f #f)
   (#f #f #f #f #f #f #t #f #f)
   (#f #f #f #f #f #f #f #t #f)
   (#f #f #f #f #f #f #f #f #t)
   (#f #f #f #f #f #f #f #f #t))

  (map (lambda (x) 
        (let ((t (map  (lambda (f) (f x)) disjoint-type-functions))) 
         t)
       ) 
   type-examples))


(test-equal "#3" 8 ((lambda (x) (+ x x)) 4))

(test-equal "#4" '(3 4 5 6) ((lambda x x) 3 4 5 6))

(test-equal "#5" '(5 6) ((lambda (x y . z) z) 3 4 5 6))

(test-equal "#6" 'yes (if (> 3 2) 'yes 'no))

(test-equal "#7" 'no (if (> 2 3) 'yes 'no))

(test-equal "#8" 1 (if (> 3 2) (- 3 2) (+ 3 2)))

(test-equal "#9" 'greater (cond ((> 3 2) 'greater) ((< 3 2) 'less)))

(test-equal "#10" 'equal (cond ((> 3 3) 'greater) ((< 3 3) 'less) (else 'equal)))

(test-equal "#11" 'composite (case (* 2 3) ((2 3 5 7) 'prime) ((1 4 6 8 9) 'composite)))

(test-equal "#12" 'consonant
    (case (car '(c d))
      ((a e i o u) 'vowel)
      ((w y) 'semivowel)
      (else 'consonant)))

(test-equal "#13" #t (and (= 2 2) (> 2 1)))

(test-equal "#14" #f (and (= 2 2) (< 2 1)))

(test-equal "#15" '(f g) (and 1 2 'c '(f g)))

(test-equal "#16" #t (and))

(test-equal "#17" #t (or (= 2 2) (> 2 1)))

(test-equal "#18" #t (or (= 2 2) (< 2 1)))

(test-equal "#19" '(b c) (or (memq 'b '(a b c)) (/ 3 0)))

(test-equal "#20" 6 (let ((x 2) (y 3)) (* x y)))

(test-equal "#21" 35 (let ((x 2) (y 3)) (let ((x 7) (z (+ x y))) (* z x))))

(test-equal "#22" 70 (let ((x 2) (y 3)) (let* ((x 7) (z (+ x y))) (* z x))))

(test-equal "#23" '#(0 1 2 3 4)
 (do ((vec (make-vector 5))
      (i 0 (+ i 1)))
     ((= i 5) vec)
   (vector-set! vec i i)))

(test-equal "#24" 25
    (let ((x '(1 3 5 7 9)))
      (do ((x x (cdr x))
           (sum 0 (+ sum (car x))))
          ((null? x)
           sum))))

(test-equal "#25" '((6 1 3) (-5 -2))
    (let loop ((numbers '(3 -2 1 6 -5)) (nonneg '()) (neg '()))
      (cond
       ((null? numbers)
        (list nonneg neg))
       ((>= (car numbers) 0)
        (loop (cdr numbers) (cons (car numbers) nonneg) neg))
       ((< (car numbers) 0)
        (loop (cdr numbers) nonneg (cons (car numbers) neg))))))

(test-equal "#26" '(list 3 4) `(list ,(+ 1 2) 4))

(test-equal "#27" '(list a 'a) (let ((name 'a)) `(list ,name ',name)))

(test-equal "#28" '(a 3 4 5 6 b)
    `(a ,(+ 1 2) ,@(map abs '(4 -5 6)) b))

;;(test-equal "#29" '(10 5 2 4 3 8)
;;    `(10 5 ,(sqrt 4) ,@(map sqrt '(16 9)) 8))

(test-equal "#30" '(a `(b ,(+ 1 2) ,(foo 4 d) e) f)
    `(a `(b ,(+ 1 2) ,(foo ,(+ 1 3) d) e) f))

(test-equal "#31" '(a `(b ,x ,'y d) e)
    (let ((name1 'x)
          (name2 'y))
      `(a `(b ,,name1 ,',name2 d) e)))

(test-equal "#32" '(list 3 4)
 (quasiquote (list (unquote (+ 1 2)) 4)))

(test-equal "#33" #t (eqv? 'a 'a))

(test-equal "#34" #f (eqv? 'a 'b))

(test-equal "#35" #t (eqv? '() '()))

(test-equal "#36" #f (eqv? (cons 1 2) (cons 1 2)))

(test-equal "#37" #f (eqv? (lambda () 1) (lambda () 2)))

(test-equal "#38" #t (let ((p (lambda (x) x))) (eqv? p p)))

(test-equal "#39" #t (eq? 'a 'a))

(test-equal "#40" #f (eq? (list 'a) (list 'a)))

(test-equal "#41" #t (eq? '() '()))

(test-equal "#42" #t (eq? car car))

(test-equal "#43" #t (let ((x '(a))) (eq? x x)))

(test-equal "#44" #t (let ((p (lambda (x) x))) (eq? p p)))

(test-equal "#45" #t (equal? 'a 'a))

(test-equal "#46" #t (equal? '(a) '(a)))

(test-equal "#47" #t (equal? '(a (b) c) '(a (b) c)))

(test-equal "#48" #t (equal? "abc" "abc"))

(test-equal "#49" #t (equal? 2 2))

(test-equal "#50" #t (equal? (make-vector 5 'a) (make-vector 5 'a)))

(test-equal "#51" 4 (max 3 4))

(test-equal "#52" 4 (max 3.9 4))

(test-equal "#53" 7 (+ 3 4))

(test-equal "#54" 3 (+ 3))

(test-equal "#55" 0 (+))

(test-equal "#56" 4 (* 4))

(test-equal "#57" 1 (*))

(test-equal "#58" -1 (- 3 4))

(test-equal "#59" -6 (- 3 4 5))

(test-equal "#60" -3 (- 3))

(test-equal "#61" 7 (abs -7))

(test-equal "#62" 1 (modulo 13 4))

(test-equal "#63" 1 (remainder 13 4))

(test-equal "#64" 3 (modulo -13 4))

(test-equal "#65" -1 (remainder -13 4))

(test-equal "#66" -3 (modulo 13 -4))

(test-equal "#67" 1 (remainder 13 -4))

(test-equal "#68" -1 (modulo -13 -4))

(test-equal "#69" -1 (remainder -13 -4))

(test-equal "#70" 4 (gcd 32 -36))

(test-equal "#71" 288 (lcm 32 -36))

;;(test-equal "#72" -5 (floor -4.3))
;;
;;(test-equal "#73" -4 (ceiling -4.3))
;;
;;(test-equal "#74" -4 (truncate -4.3))
;;
;;(test-equal "#75" -4 (round -4.3))
;;
;;(test-equal "#76" 3 (floor 3.5))
;;
;;(test-equal "#77" 4 (ceiling 3.5))
;;
;;(test-equal "#78" 3 (truncate 3.5))
;;
;;(test-equal "#79" 4 (round 3.5))

(test-equal "#80" 100 (string->number "100"))

;(test-equal "#81" 256 (string->number "100" 16))

(test-equal "#82" 100.0 (string->number "1e2"))

(test-equal "#83" #f (not 3))

(test-equal "#84" #f (not (list 3)))

(test-equal "#85" #f (not '()))

(test-equal "#86" #f (not (list)))

(test-equal "#87" #f (not '()))

(test-equal "#88" #f (boolean? 0))

(test-equal "#89" #f (boolean? '()))

(test-equal "#90" #t (pair? '(a . b)))

(test-equal "#91" #t (pair? '(a b c)))

(test-equal "#92" '(a) (cons 'a '()))

(test-equal "#93" '((a) b c d) (cons '(a) '(b c d)))

(test-equal "#94" '("a" b c) (cons "a" '(b c)))

(test-equal "#95" '(a . 3) (cons 'a 3))

(test-equal "#96" '((a b) . c) (cons '(a b) 'c))

(test-equal "#97" 'a (car '(a b c)))

(test-equal "#98" '(a) (car '((a) b c d)))

(test-equal "#99" 1 (car '(1 . 2)))

(test-equal "#100" '(b c d) (cdr '((a) b c d)))

(test-equal "#101" 2 (cdr '(1 . 2)))

(test-equal "#102" #t (list? '(a b c)))

(test-equal "#103" #t (list? '()))

(test-equal "#104" #f (list? '(a . b)))

(test-equal "#105" #f
    (let ((x (list 'a)))
      (set-cdr! x x)
      (list? x)))

(test-equal "#106" '(a 7 c) (list 'a (+ 3 4) 'c))

(test-equal "#107" '() (list))

(test-equal "#108" 3 (length '(a b c)))

(test-equal "#109" 3 (length '(a (b) (c d e))))

(test-equal "#110" 0 (length '()))

(test-equal "#111" '(x y) (append '(x) '(y)))

(test-equal "#112" '(a b c d) (append '(a) '(b c d)))

(test-equal "#113" '(a (b) (c)) (append '(a (b)) '((c))))

(test-equal "#114" '(a b c . d) (append '(a b) '(c . d)))

(test-equal "#115" 'a (append '() 'a))

(test-equal "#116" '(c b a) (reverse '(a b c)))

(test-equal "#117" '((e (f)) d (b c) a) (reverse '(a (b c) d (e (f)))))

(test-equal "#118" 'c (list-ref '(a b c d) 2))

(test-equal "#119" '(a b c) (memq 'a '(a b c)))

(test-equal "#120" '(b c) (memq 'b '(a b c)))

(test-equal "#121" #f (memq 'a '(b c d)))

(test-equal "#122" #f (memq (list 'a) '(b (a) c)))

(test-equal "#123" '((a) c) (member (list 'a) '(b (a) c)))

(test-equal "#124" '(101 102) (memv 101 '(100 101 102)))

(test-equal "#125" #f (assq (list 'a) '(((a)) ((b)) ((c)))))

(test-equal "#126" '((a)) (assoc (list 'a) '(((a)) ((b)) ((c)))))

(test-equal "#127" '(5 7) (assv 5 '((2 3) (5 7) (11 13))))

(test-equal "#128" #t (symbol? 'foo))

(test-equal "#129" #t (symbol? (car '(a b))))

(test-equal "#130" #f (symbol? "bar"))

(test-equal "#131" #t (symbol? 'nil))

(test-equal "#132" #f (symbol? '()))

(test-equal "#133" "flying-fish" (symbol->string 'flying-fish))

;(test-equal "#134" "Martin" (symbol->string 'Martin))

(test-equal "#135" "Malvina" (symbol->string (string->symbol "Malvina")))

(test-equal "#136" '#(0 ("Sue" "Sue") "Anna")
 (let ((vec (vector 0 '(2 2 2 2) "Anna")))
   (vector-set! vec 1 '("Sue" "Sue"))
   vec))

(test-equal "#137" '(dah dah didah) (vector->list '#(dah dah didah)))

(test-equal "#138" '#(dididit dah) (list->vector '(dididit dah)))

(test-equal "#139" #t (procedure? car))

(test-equal "#140" #f (procedure? 'car))

(test-equal "#141" #t (procedure? (lambda (x) (* x x))))

(test-equal "#142" #f (procedure? '(lambda (x) (* x x))))

(test-equal "#143" #t (call-with-current-continuation procedure?))

(test-equal "#144" 7 (apply + (list 3 4)))

(test-equal "#145" '(b e h) (map cadr '((a b) (d e) (g h))))

;; (test-equal "#146" '(1 4 27 256 3125) (map (lambda (n) (expt n n)) '(1 2 3 4 5)))

(test-equal "#147" '(5 7 9) (map + '(1 2 3) '(4 5 6)))

(test-equal "#148" '#(0 1 4 9 16)
    (let ((v (make-vector 5)))
      (for-each
       (lambda (i) (vector-set! v i (* i i)))
       '(0 1 2 3 4))
      v))

(test-equal "#149" 3 (force (delay (+ 1 2))))

(test-equal "#150" '(3 3) (let ((p (delay (+ 1 2)))) (list (force p) (force p))))

(test-equal "#151 (apply append)" '() (apply append '(())))
(test-equal "#152 (append '())"   '() (append '()))
(test-equal "#153 (append 3)"     3 (append 3))
(test-equal "#154 (reverse '())"  '() (reverse '()))


(run-all-tests "R5RS Tests (without math)")
