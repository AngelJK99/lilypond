\version "2.11.33"
#(define  (naturalise-pitch p)
  (let* ((o (ly:pitch-octave p))
         (a (* 4 (ly:pitch-alteration p))) 
    ; alteration, a, in quarter tone steps, for historical reasons
         (n (ly:pitch-notename p)))

    (cond
     ((and (> a 1) (or (eq? n 6) (eq? n 2)))
      (set! a (- a 2))
      (set! n (+ n 1)))
     ((and (< a -1) (or (eq? n 0) (eq? n 3)))
      (set! a (+ a 2))
      (set! n (- n 1))))

    (cond
     ((> a 2) (set! a (- a 4)) (set! n (+ n 1)))
     ((< a -2) (set! a (+ a 4)) (set! n (- n 1))))

    (if (< n 0) (begin (set!  o (- o 1)) (set! n (+ n 7))))
    (if (> n 6) (begin (set!  o (+ o 1)) (set! n (- n 7))))

    (ly:make-pitch o n (/ a 4))))

#(define (naturalise music)
  (let* ((es (ly:music-property music 'elements))
         (e (ly:music-property music 'element))
         (p (ly:music-property music 'pitch)))

    (if (pair? es)
        (ly:music-set-property!
         music 'elements
         (map (lambda (x) (naturalise x)) es)))

    (if (ly:music? e)
        (ly:music-set-property!
         music 'element
         (naturalise e)))

    (if (ly:pitch? p)
        (begin
          (set! p (naturalise-pitch p))
          (ly:music-set-property! music 'pitch p)))

    music))

music =  \relative c' { c4 d  e f g a b  c }

naturaliseMusic =
#(define-music-function (parser location m)
					(ly:music?)
			(naturalise m))

\score {
   \new Staff {
     \transpose c ais \music
     \naturaliseMusic \transpose c ais \music 
    \break
    \transpose c deses \music
    \naturaliseMusic \transpose c deses \music
  }
  \layout { ragged-right = ##t}
}


