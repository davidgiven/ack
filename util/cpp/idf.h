struct id_usr {
	struct macro *idu_macro;
	int idu_resmac;
};

#define IDF_TYPE struct id_usr
#define IDF_HSIZE 6
#define id_macro id_user.idu_macro
#define id_resmac id_user.idu_resmac

#include <idf_pkg.spec>
