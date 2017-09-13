\header {

  texidoc = "\\consists can take a scheme alist as arguments, which
  should be functions, which will be invoked as engraver functions."

}

\version "2.19.21"

#(define (t->m t)
   "Return the current moment of translator object @var{t}."
   (ly:context-current-moment (ly:translator-context t)))

engraver_demo =
#(make-engraver
   ((initialize translator)
    (format (current-error-port) "\n\n~16a: (initialize)\n" (t->m translator)))
   ((start-translation-timestep translator)
    (format (current-error-port) "~16a: (start-translation-timestep)\n" (t->m translator)))
   (listeners
     ((rest-event engraver event)
      (let ((grob (ly:engraver-make-grob engraver 'TextScript event)))
        (ly:grob-set-property! grob 'text "hi")
        (format (current-error-port) "~16a: detected this rest event: ~a\n~16a: created this grob: ~a\n"
                (t->m engraver) event (t->m engraver) grob))))
   (acknowledgers
     ((note-head-interface engraver grob source-engraver)
      (format (current-error-port) "~16a: saw ~a coming from ~a\n"
              (t->m engraver) grob source-engraver)))
   (end-acknowledgers
     ((beam-interface engraver grob source-engraver)
      (format (current-error-port) "~16a: saw end of ~a coming from ~a\n"
              (t->m engraver) grob source-engraver)))
   ((process-music translator)
    (format (current-error-port) "~16a: (process-music)\n" (t->m translator)))
   ((process-acknowledged translator)
    (format (current-error-port) "~16a: (process-acknowledged)\n" (t->m translator)))
   ((stop-translation-timestep translator)
    (format (current-error-port) "~16a: (stop-translation-timestep)\n" (t->m translator)))
   ((finalize translator)
    (format (current-error-port) "~16a: (finalize)\n" (t->m translator))))

\layout {
  \context {
    \Voice
    \consists
    \engraver_demo
  }
}

\relative {
  c'8[ r c]
}
