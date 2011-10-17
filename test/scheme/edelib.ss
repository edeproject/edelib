;; vim:fenc=utf-8
;; edelib specific extensions
;;

(load "test.ss")

(test-equal "#1" _"This is some text" "This is some text")
(test-equal "#2" "Ја могу да једем стакло. " "Ја могу да једем стакло. ")
;(test-equal "#3" (string-length _"Ја могу да једем стакло. ") 25)

(run-all-tests "edelib specific Tests")
