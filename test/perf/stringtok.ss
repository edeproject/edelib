;;
;; chicken scheme comparison
;; (compare it against either interpreted or compiled version)
;;
;; Run:
;;   csi -bq stringtok.ss
;; or to compile:
;;   csc -O2 stringtok.ss

(define *split-token* "</")

(define (manage-file)
 (let* ([tokens      #f]
		;; read all lines
		[content     (read-lines (current-input-port) 10000)]
	    ;; 'string-intersperse' will convert a list of lines into reasonable string
		;; this is a chicken extension
	    [content-str (string-intersperse content)])	

  (set! tokens (string-split content-str *split-token*))
  (printf "sz: ~A~%~%" (length tokens))
 )
)

;; timeit
(time (with-input-from-file "asciidoc.html" manage-file))
