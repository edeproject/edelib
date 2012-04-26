
;;; documentation generator, usable from doxygen

(define (doc-data-sorted)
  (sort-with-operator *doc-data*
					  (lambda (x y)
						(string<? (vector-ref x 0)
								  (vector-ref y 0) ) ) ) )

(define (char->string c)
  (anyatom->string c char?))

(define (htmlify str)
  (let1 ret ""
    (for i in str
	  (cond
	    [(char=? i #\>)
		 (set! ret (string-append ret "&gt;"))]
		[(char=? i #\<)
		 (set! ret (string-append ret "&lt;"))]
		[(char=? i #\&)
		 (set! ret (string-append ret "&amp;"))]
		[else
		 (set! ret (->> (char->string i)
						(string-append ret) ) ) ] ) )
	ret))


(define (doc->doxygen)
  (let1 sorted (doc-data-sorted)
    ;; generate TOC first
	(println "<ul>")

	(for-each
	  (lambda (x)
		(let1 name (htmlify (vector-ref x 0))
	      (println "<li><a href=#" name ">" name "</a></li>") ) )
	  sorted)

	(println "</ul>")

	(for-each
	  (lambda (x)
		(let2 name  (htmlify (vector-ref x 0))
			  descr (vector-ref x 1)
		  (println "<h1><a name=\"" name "\">" name "</a></h1>")
		  (println (vector-ref x 2))
		  (println "<p>" descr "</p><br/>")
		  (println "<hr/>")))
	  sorted ) ) )

(doc->doxygen)
