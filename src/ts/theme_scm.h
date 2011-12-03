/* Generated with gen-c-string.sh. Do not edit this file, edit .scm file */
static const char theme_scm_content[] =
    " (define *edelib-dir-separator*    #f) "
    " (define private:theme.search-path #f) "
    " (define private:theme.author #f) "
    " (define private:theme.sample #f) "
    " (define private:theme.name   #f) "
    " (define private:theme.styles '()) "
    " (define (pair-items lst) "
    "   (if (list? lst) "
    "     (let* ([vec (list->vector lst)] "
    "            [sz  (vector-length vec)] "
    "            [ret '()]) "
    "       (let loop ([i 0] "
    "                  [j 1]) "
    "         (when (< j sz) "
    "           (set! ret  "
    "             (cons  "
    "               (list (vector-ref vec i)  "
    "                     (vector-ref vec j)) "
    "               ret )) "
    "           (loop (+ i 2) "
    "                 (+ j 2) "
    "       ) ) )  "
    "       (reverse ret)) "
    "     '() "
    " ) ) "
    " (define (theme.path-resolve item)  "
    "  (if (and *edelib-dir-separator* "
    "           private:theme.search-path)  "
    "   (string-append private:theme.search-path *edelib-dir-separator* item) "
    "    item  "
    " )) "
    " (define (include file) "
    "  (load (theme.path-resolve file)) "
    " ) "
    " (define (theme.print msg . args) "
    "  (display msg) "
    "  (for-each (lambda (x) (display x)) args) "
    " ) "
    " (define (theme.warning msg . args) "
    "  (display \"*** ede theme warning: \") "
    "  (display msg) "
    "  (for-each (lambda (x) (display x)) args) "
    " ) "
    " (define (theme.error msg . args) "
    "  (newline) "
    "  (display \"*** ede theme error: \") "
    "  (display msg) "
    "  (for-each (lambda (x) (display x)) args) "
    "  (newline) "
    " ) "
    " (if (defined? 'private:theme.error_hook) "
    "     (define *error-hook* private:theme.error_hook) "
    "     (define *error-hook* theme.error)) "
    " (define (theme.author a) "
    "  (set! private:theme.author a)) "
    " (define (theme.author-get) "
    "  private:theme.author) "
    " (define (theme.sample s) "
    "  (set! private:theme.sample (theme.path-resolve s)) "
    " ) "
    " (define (theme.sample-get) "
    "   private:theme.sample) "
    " (define (theme.name n) "
    "  (set! private:theme.name n)) "
    " (define (theme.name-get) "
    "  private:theme.name) "
    " (define-macro (theme.style name . body)  "
    "  `(set! private:theme.styles  "
    "     (cons  "
    "       (list ,name (pair-items ',@body))  "
    "       private:theme.styles "
    " ) ) ) "
    " (define-with-return (theme.style-get name) "
    "  (for-each (lambda (x) "
    "              (if (string=? name (car x)) "
    "               (return (cadr x)) )) "
    "  private:theme.styles "
    " )) "
;
