;; vim:fenc=utf-8
;; edelib specific extensions
;;

(load "test.ss")

(test-equal "#1" _"This is some text" "This is some text")
(test-equal "#2" "Ја могу да једем стакло. " "Ја могу да једем стакло. ")

;; theming code
(theme.style "foo" [
  var1 "value1"
  var2 "value2"
  var3 "val3"
  num  (+ 1 2 3) ;; evaluated only when called from Theme class
])

(test-equal "#3" (theme.style-get "foo") 
                 '[(var1 "value1") 
                   (var2 "value2") 
                   (var3 "val3") 
                   (num (+ 1 2 3)) ])

(run-all-tests "edelib specific Tests")
