(define a (list "ab" "bc" "ca"))
(define pi (list 3 1 4 1 5 9 2 6 5 3))

(define test0 (lambda (a) a))

(define test1 (lambda (a)
		(let ((x (* a a)))
		  x)))

(define test2 (lambda (a)
		(letrec (
		      (loop (lambda (x)
			      (if (null? x)
				  (display "\n")
				  (begin
				    (display (car x))
				    (loop (cdr x)))
				  ))))
		  (loop a))))

(define (foldl func initial list)
  (if (null? list)
      initial
      (foldl func (func (car list) initial) (cdr list))))

(define (test a b c)
  (let* (
	 (l (list a b c))
	 (total (foldl + 0 l))
	 (dnzero (lambda (x)
		   (if (> x 0)
		       (display x))))
	 )
    (begin
      (map dnzero l)
      (display "\n")
      )
    )
  )

(define (fish l1 l2)
  (if (null? l1)
      (list)
      (letrec (
	       (loop (lambda (a b c d)
		       (begin
			 (display 
			  (if (and (> (foldl + 0 a) 0) (> b 0))
			      (if (> (foldl + 0 c) 0)
				  ", "
				  " and ")
			      ""))
			 (if (not (= b 0))
			     (begin
			       (display b)
			       (display " ")
			       (display (car d))
			       )
			     )
			 (if (not (null? c))
			     (loop (append a (list b)) (car c) (cdr c) (cdr d))
			     )
			 ))))
	(loop (list) (car l1) (cdr l1) l2)
	)
      )
  (display "\n")
  )
