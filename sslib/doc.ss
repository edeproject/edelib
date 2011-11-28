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

;; each element will be in form (vector <func name> <doc> <type>))
;; where <type> can be in form "function" "variable" or "macro"
(define *doc-data* '())

(define (add-doc-generic func str type)
  (set! *doc-data* (cons (vector func str type) *doc-data*)) )

(define (add-doc func str)
  (add-doc-generic func str "function"))

(define (add-macro-doc func str)
  (add-doc-generic func str "macro"))

(define (add-var-doc func str)
  (add-doc-generic func str "variable"))

(define-with-return (find-doc func)
  (unless (string? func) (error _"find-doc expects string argument"))

  (for-each (lambda (x)
              (if (string=? func (vector-ref x 0))
                (return (vector-ref x 1)) ))
            *doc-data*)
  #f)

(define (doc func)
  (define ret (find-doc func))
  (if ret
    (begin
      (display func)
      (display "\n----------\n")
      (display ret))
    (display _"Not found"))
  (newline))

(add-doc "find-doc" "Try to find documentation for given parameter. Return documentation string or #f if fails.")
(add-doc "doc" "Display documentation for given function name string.")
(add-doc "add-doc" "Add documentation for given functioa.n")
(add-doc "add-macro-doc" "Add documentation for macro.")
(add-doc "add-var-doc" "Add documentation for variable.")
