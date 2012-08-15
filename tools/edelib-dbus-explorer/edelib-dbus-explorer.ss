;;
;; $Id: String.h 2839 2009-09-28 11:36:20Z karijes $
;;
;; Copyright (c) 2005-2012 edelib authors
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

;; custom scheme code loaded when explorer is started

;; dbus-call-raw has explicit parameters as list and this macro will implicitly convert remaining parameters to list
(add-macro-doc "dbus-call" "Call DBus method with given arguments. This call will wait for reply and return result as scheme object.")
(define-macro (dbus-call service path interface name . args)
  `(if (empty? ',args)
     (dbus-call-raw ,service ,path ,interface ,name)
	 (dbus-call-raw ,service ,path ,interface ,name ',args)))
