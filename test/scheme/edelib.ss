;;
;; edelib specific extensions
;;

(load "test.ss")

(test-equal "#1" _"This is some text" "This is some text")

(run-all-tests "edelib specific Tests")
