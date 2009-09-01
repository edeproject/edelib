;;
;; while macro
;;

(define-macro (while . body)
  `(let loop ()
	 ;; fetch condition
	 (if ,(car body)
	   (begin
		 ;; evaluate body
		 ,@(cdr body)
		 (loop)))))

(define i 0)

(while (< i 10000)
  (display i)
  (newline)
  (set! i (+ i 1))
)
