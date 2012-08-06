;; vim:ft=scheme:expandtab
;; $Id: String.h 2839 2009-09-28 11:36:20Z karijes $
;;
;; Copyright (c) 2012 edelib authors
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

(define lazy-car car)

(define (lazy-cdr lst)
  (force (cdr lst)))

(define lazy-first lazy-car)
(define lazy-rest lazy-cdr)

(add-macro-doc "lazy-cons" "Lazy cons.")
(define-macro (lazy-cons item lst)
  `(cons ,item (delay ,lst)))

(defun lazy-range (s e)
  "Lazy range starting from s and ending with e - 1."
  (let loop ([s s]
             [e e])
    (if (>= s e)
      '()
      (lazy-cons s (loop (+ 1 s) e)) )))

(defun lazy-take (n lst)
  "Take n elements from lazy list in lazy fashion. Taken elements will
   be realized."
  (if (or (equal? lst '())
          (<= n 0))
    '()
    (cons (lazy-car lst)
          (lazy-take (- n 1) (lazy-cdr lst)) ) ) )

(defun lazy-filter (pred seq)
  "Filter lazy sequence with given predicate. The function will do actual work
   when elements are requested with e.g. lazy-take."
  (cond
    [(null? seq) seq]
    [(pred (lazy-car seq))
     (lazy-cons (car seq)
                (lazy-filter pred (lazy-cdr seq)) )]
    [else
      (lazy-filter pred (lazy-cdr seq)) ]))

(defun lazy-map (fn . lists)
  "Lazy map."
  (if (memq '() lists)
    '()
    (lazy-cons (apply fn (map lazy-car lists))
               (apply lazy-map fn (map lazy-cdr lists)) ) ) )

(defun lazy-nth (n lst)
  "Return n item of lazy list. All elements up to this position, including this element
   will be realized."
  (if (<= n 0)
    (lazy-car lst)
    (lazy-nth (- n 1) (lazy-cdr lst)) ) )
