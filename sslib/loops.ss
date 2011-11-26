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

(define-macro (for var _ collection . body)
  `(cond
     ((list? ,collection)
      (map (lambda (,var) 
             ,@body) 
           ,collection))
     ((vector? ,collection)
      (map (lambda (,var)
             ,@body)
           (vector->list ,collection) ))
     (else
       (throw "Unsupported type in 'for' loop") )))

(define-macro (while expr . body)
  `(let loop ()
     (when ,expr
       ,@body
       (loop) )))
