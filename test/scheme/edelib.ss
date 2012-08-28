;; vim:fenc=utf-8
;; edelib specific extensions
;;

(load "test.ss")

(test-equal "translation char" _"This is some text" "This is some text")
(test-equal "utf-8 chars" "Ја могу да једем стакло. " "Ја могу да једем стакло. ")

;; theming code
(theme.style "foo" [
  var1 "value1"
  var2 "value2"
  var3 "val3"
  num  (+ 1 2 3) ;; evaluated only when called from Theme class
])

(test-equal "theme.style-get" (theme.style-get "foo") 
                              '[(var1 "value1") 
                                (var2 "value2") 
                                (var3 "val3") 
                                (num (+ 1 2 3)) ])

(run-all-tests "edelib specific Tests")
