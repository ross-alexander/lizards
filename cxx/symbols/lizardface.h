
 typedef struct _LizardFace            LizardFace;
 typedef struct _LizardFaceClass       LizardFaceClass;

 struct _LizardFace
 {
        GtkDrawingArea parent;

        /* private */
 };

 struct _LizardFaceClass
 {
        GtkDrawingAreaClass parent_class;
 };


#define TYPE_LIZARD_FACE             (lizard_face_get_type ())
#define LIZARD_FACE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), EGG_TYPE_CLOCK_FACE, EggClockFace))
#define LIZARD_FACE_CLASS(obj)       (G_TYPE_CHECK_CLASS_CAST ((obj), LIZARD_FACE,  EggClockFaceClass))
#define EGG_IS_CLOCK_FACE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EGG_TYPE_CLOCK_FACE))
#define EGG_IS_CLOCK_FACE_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), EGG_TYPE_CLOCK_FACE))
#define LIZARD_FACE_GET_CLASS        (G_TYPE_INSTANCE_GET_CLASS ((obj), EGG_TYPE_CLOCK_FACE, EggClockFaceClass))

