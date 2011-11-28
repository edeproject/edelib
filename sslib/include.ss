;; vim:ft=scheme:expandtab
;; $Id: String.h 2839 2009-09-28 11:36:20Z karijes $
;;
;; Copyright (c) 2005-2011 edelib authors
;;
;; This library is free software; you can redistribute it and/or
;; modify it under the terms of the GNU Lesser General Public
;; License as published by the Free Software Foundation; either
;; version 2 of the License, or (at your option) any later version.
;;
;; This library is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
;; Lesser General Public License for more details.
;;
;; You should have received a copy of the GNU Lesser General Public License
;; along with this library. If not, see <http://www.gnu.org/licenses/>.

;; basic functions for handling 'include' statement
(or (defined? '*include-path*)
    (define *include-path* (list)))

(define (add-to-include-path path)
  (set! *include-path*
    (cons path *include-path*)))

(define (remove-from-include-path path)
  (let ([n (list)])
    (for-each (lambda (p)
                (if (not (= p path))
                  (set! n (cons p n)) ))
              *include-path*)
    (set! *include-path* n) 
) )

(define-with-return (include object)
  (define (file-exists f)
    (let* ([fd   (open-input-file f)]
           [ret  (if fd #t #f)])
      (if fd
        (close-input-port fd))
      ret))

  (for-each (lambda (x)
    (define full-path (string-append x "/" object))

    (when (file-exists full-path)
      (load full-path)
      (return #t) ))

    *include-path*
) )
