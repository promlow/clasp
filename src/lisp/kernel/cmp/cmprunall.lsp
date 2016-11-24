(in-package :cmp)



(defvar *generate-compile-file-load-time-values* nil
  "This variable controls whether literals are compiled into the
load-time-value manager (true - in COMPILE-FILE) or not (false - in COMPILE)."
)



;;; Contains the current RUN-ALL, initialization function
;;; for the current module
(defvar *run-all-function*)
(defvar +run-and-load-time-value-holder-global-var-type+ cmp:+ltv*+) ;; Was +ltvsp*+
(defvar *run-time-values-table-name* "run_time_values_table")
(defvar *run-all-environment*)
;;;------
;;; Set up the run-time-values-table
;;;  set-run-time-values-table MUST be called to set the
;;;  global
(defvar *run-time-values-table* (core:load-time-value-array *run-time-values-table-name* 0))
(core:set-run-time-values-table *run-time-values-table-name*)

(defvar *load-time-value-holder-global-var* nil
  "Store the current load-time-value data structure for COMPILE-FILE")

(defvar *run-time-values-table-global-var* nil
  "All load-time-values and quoted values are stored in this array accessed with an integer index"
  )


(defvar *irbuilder-run-all-alloca* nil
  "Maintains an IRBuilder for the load-time-value function alloca area")
(defvar *irbuilder-run-all-body* nil
  "Maintain an IRBuilder for the load-time-value body area")

(defvar *run-all-result*)

(defmacro with-make-new-run-all ((run-all-fn) &body body)
  "Set up a run-all function in the current module, return the run-all-fn"
  (let ((irbuilder-alloca (gensym "ltv-irbuilder-alloca"))
	(irbuilder-body (gensym "ltv-irbuilder-body")))
    `(let ((,run-all-fn (irc-simple-function-create core:+run-all-function-name+
                                                    +fn-prototype+
                                                    'llvm-sys:internal-linkage
                                                    *the-module*
                                                    :argument-names +fn-prototype-argument-names+))
           (,irbuilder-alloca (llvm-sys:make-irbuilder *llvm-context*))
           (,irbuilder-body (llvm-sys:make-irbuilder *llvm-context*)))
       (let* ((*run-all-function* ,run-all-fn)
              (*irbuilder-run-all-alloca* ,irbuilder-alloca)
              (*irbuilder-run-all-body* ,irbuilder-body)
              (*current-function* ,run-all-fn)
              (*run-all-objects* nil))
         (cmp:with-dbg-function ("runAll-dummy-name"
                                 :linkage-name (llvm-sys:get-name ,run-all-fn)
                                 :function ,run-all-fn
                                 :function-type cmp:+fn-prototype+
                                 :form nil) ;; No form for run-all
           ;; Set up dummy debug info for these irbuilders
           (let ((entry-bb (irc-basic-block-create "entry" ,run-all-fn)))
             (llvm-sys:set-insert-point-basic-block ,irbuilder-alloca entry-bb))
           (let ((body-bb (irc-basic-block-create "body" ,run-all-fn)))
             (llvm-sys:set-insert-point-basic-block ,irbuilder-body body-bb)
             ;; Setup exception handling and cleanup landing pad
             (with-irbuilder (,irbuilder-alloca)
               (let ((entry-branch (irc-br body-bb)))
                 (llvm-sys:set-insert-point-instruction ,irbuilder-alloca entry-branch)
                 (with-irbuilder (,irbuilder-body)
                   (progn ,@body)
                   (irc-ret (irc-undef-value-get +tmv+))))))))
       (values ,run-all-fn))))


(defmacro with-run-all-entry-codegen (&body form)
  "Generate code within the ltv-function - used by codegen-load-time-value"
  `(let ((*irbuilder-function-alloca* *irbuilder-run-all-alloca*)
	 (*current-function* *run-all-function*))
     (cmp:with-irbuilder (*irbuilder-run-all-alloca*)
       ,@form)))

(defmacro with-run-all-body-codegen ( &body form)
  "Generate code within the ltv-function - used by codegen-load-time-value"
  `(let ((*irbuilder-function-alloca* *irbuilder-run-all-alloca*)
	 (*current-function* *run-all-function*))
     (cmp:with-irbuilder (*irbuilder-run-all-body*)
       ,@form)))

(defun ltv-global ()
  "called by cclasp"
  *load-time-value-holder-global-var*
  #+(or)(if cmp:*generate-compile-file-load-time-values*
      *load-time-value-holder-global-var*
      *run-time-values-table-global-var*))

(defun generate-load-time-values () *generate-compile-file-load-time-values*)
