
(test-equal "Test string?" #t (and (string? "Sample string")
                                   (not (string? 1234))
                                   (not (string? #\w))))

(test-equal "Test string=?" #t (and (string=? "foo" "foo")
                                    (string=? "FOO" "FOO")
                                    (not (string=? "FoO" "FOO"))))

(test-equal "Test string<?" #t (and (string<? "aaaa" "z")
                                     (string<? "foo" "fooo")
                                     (not (string<? "foo" "assadsdasds"))
                                     (not (string<? "foooooooo" "ab"))))

(test-equal "Test string>?" #t (and (string>? "z" "aaa") 
                                    (string>? "fooo" "foo") 
                                    (string>? "foo" "assadsdasds") 
                                    (string>? "foooooooooo" "Ab")))

(test-equal "Test string<=?" #t (and (string<=? "aaa" "z")
                                     (string<=? "aaaa" "aaaa")
                                     (string<=? "" "")
                                     (not (string<=? "foo" "assadsdasds"))
                                     (not (string<=? "fooooooooo" "ab"))))

(test-equal "Test string>=?" #t (and (string>=? "z" "aaa")
                                     (string>=? "aaaa" "aaaa")
                                     (string>=? "" "")
                                     (string>=? "foo" "assadsdasds")
                                     (string>=? "foo" "Foo")
                                     (string>=? "foo" "fOO")))

(test-equal "Test list->string" "sample string" (list->string '(#\s #\a #\m #\p #\l #\e #\space #\s #\t #\r #\i #\n #\g)))

(test-equal "Test string-length" #t (and (= 18 (string-length "some stupid sample"))
                                         (= 0  (string-length ""))
                                         (= 1  (string-length "a"))))
