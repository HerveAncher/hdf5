/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by The HDF Group.                                               *
 * Copyright by the Board of Trustees of the University of Illinois.         *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of HDF5.  The full HDF5 copyright notice, including     *
 * terms governing use, modification, and redistribution, is contained in    *
 * the COPYING file, which can be found at the root of the source code       *
 * distribution tree, or in https://support.hdfgroup.org/ftp/HDF5/releases.  *
 * If you do not have access to either file, you may request a copy from     *
 * help@hdfgroup.org.                                                        *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/****************/
/* Module Setup */
/****************/

#include "H5Rmodule.h"          /* This source code file is part of the H5R module */


/***********/
/* Headers */
/***********/
#include "H5private.h"          /* Generic Functions                        */
#include "H5ACprivate.h"        /* Metadata cache                           */
#include "H5Dprivate.h"         /* Datasets                                 */
#include "H5Eprivate.h"         /* Error handling                           */
#include "H5Gprivate.h"         /* Groups                                   */
#include "H5HGprivate.h"        /* Global Heaps                             */
#include "H5Iprivate.h"         /* IDs                                      */
#include "H5MMprivate.h"        /* Memory management                        */
#include "H5Oprivate.h"         /* Object headers                           */
#include "H5Aprivate.h"         /* Attributes                               */
#include "H5Tprivate.h"         /* Datatypes                                */
#include "H5Rpkg.h"             /* References                               */


/****************/
/* Local Macros */
/****************/

#define H5R_MAX_ATTR_REF_NAME_LEN (64 * 1024)

/******************/
/* Local Typedefs */
/******************/

/********************/
/* Local Prototypes */
/********************/


/*********************/
/* Package Variables */
/*********************/

/* Package initialization variable */
hbool_t H5_PKG_INIT_VAR = FALSE;

/*****************************/
/* Library Private Variables */
/*****************************/

/*******************/
/* Local Variables */
/*******************/

/* Reference ID class
 *
 * NOTE: H5I_REFERENCE is not used by the library and has been deprecated
 *       with a tentative removal version of 1.12.0. (DER, July 2017)
 */
static const H5I_class_t H5I_REFERENCE_CLS[1] = {{
    H5I_REFERENCE,          /* ID class value                                       */
    0,                      /* Class flags                                          */
    0,                      /* # of reserved IDs for class                          */
    (H5I_free_t) H5R_destroy/* Callback routine for closing objects of this class   */
}};

/* Flag indicating "top" of interface has been initialized */
static hbool_t H5R_top_package_initialize_s = FALSE;


/*--------------------------------------------------------------------------
NAME
   H5R__init_package -- Initialize interface-specific information
USAGE
    herr_t H5R__init_package()

RETURNS
    Non-negative on success/Negative on failure
DESCRIPTION
    Initializes any interface-specific data or routines.

--------------------------------------------------------------------------*/
herr_t
H5R__init_package(void)
{
    herr_t ret_value = SUCCEED;         /* Return value */

    FUNC_ENTER_NOAPI_NOINIT

    /* Initialize the atom group for the file IDs */
    if (H5I_register_type(H5I_REFERENCE_CLS) < 0)
        HGOTO_ERROR(H5E_REFERENCE, H5E_CANTINIT, FAIL, "unable to initialize interface")

    /* Mark "top" of interface as initialized, too */
    H5R_top_package_initialize_s = TRUE;

done:
    FUNC_LEAVE_NOAPI(ret_value)
} /* end H5R__init_package() */


/*--------------------------------------------------------------------------
 NAME
    H5R_top_term_package
 PURPOSE
    Terminate various H5R objects
 USAGE
    void H5R_top_term_package()
 RETURNS
    void
 DESCRIPTION
    Release IDs for the atom group, deferring full interface shutdown
    until later (in H5R_term_package).
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
     Can't report errors...
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
int
H5R_top_term_package(void)
{
    int	n = 0;

    FUNC_ENTER_NOAPI_NOINIT_NOERR

    if (H5R_top_package_initialize_s) {
        if (H5I_nmembers(H5I_REFERENCE) > 0) {
            (void)H5I_clear_type(H5I_REFERENCE, FALSE, FALSE);
            n++;
	    }

        /* Mark closed */
        if (0 == n)
            H5R_top_package_initialize_s = FALSE;
    }

    FUNC_LEAVE_NOAPI(n)
} /* end H5R_top_term_package() */


/*--------------------------------------------------------------------------
 NAME
    H5R_term_package
 PURPOSE
    Terminate various H5R objects
 USAGE
    void H5R_term_package()
 RETURNS
    void
 DESCRIPTION
    Release the atom group and any other resources allocated.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
     Can't report errors...

     Finishes shutting down the interface, after H5R_top_term_package()
     is called
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
int
H5R_term_package(void)
{
    int	n = 0;

    FUNC_ENTER_NOAPI_NOINIT_NOERR

    if (H5_PKG_INIT_VAR) {
        /* Sanity checks */
        HDassert(0 == H5I_nmembers(H5I_REFERENCE));
        HDassert(FALSE == H5R_top_package_initialize_s);

        /* Destroy the reference id group */
        n += (H5I_dec_type_ref(H5I_REFERENCE) > 0);

        /* Mark closed */
        if (0 == n)
            H5_PKG_INIT_VAR = FALSE;
    }

    FUNC_LEAVE_NOAPI(n)
} /* end H5R_term_package() */


/*-------------------------------------------------------------------------
 * Function:    H5R_create_object
 *
 * Purpose: Creates an object reference. The LOC and NAME are used to locate
 * the object pointed to.
 *
 * Return:  Success:    Reference created
 *          Failure:    NULL
 *
 *-------------------------------------------------------------------------
 */
href_t
H5R_create_object(H5G_loc_t *loc, const char *name)
{
    H5G_loc_t obj_loc;          /* Group hier. location of object */
    H5G_name_t path;            /* Object group hier. path */
    H5O_loc_t oloc;             /* Object object location */
    hbool_t obj_found = FALSE;  /* Object location found */
    struct href *ret_value = NULL; /* Return value */

    FUNC_ENTER_PACKAGE_VOL

    HDassert(loc);
    HDassert(name);

    /* Set up object location to fill in */
    obj_loc.oloc = &oloc;
    obj_loc.path = &path;
    H5G_loc_reset(&obj_loc);

    /* Find the object */
    if(H5G_loc_find(loc, name, &obj_loc) < 0)
        HGOTO_ERROR(H5E_REFERENCE, H5E_NOTFOUND, NULL, "object not found")
    obj_found = TRUE;

    if(NULL == (ret_value = (struct href *)H5MM_malloc(sizeof(struct href))))
        HGOTO_ERROR(H5E_RESOURCE, H5E_NOSPACE, NULL, "Cannot allocate memory for reference")

    ret_value->ref_type = H5R_OBJECT;
    ret_value->ref.addr = obj_loc.oloc->addr;

done:
    if(obj_found)
        H5G_loc_free(&obj_loc);

    FUNC_LEAVE_NOAPI_VOL(ret_value)
} /* end H5R_create_object() */

/*-------------------------------------------------------------------------
 * Function:    H5R_create_region
 *
 * Purpose: Creates a region reference. The LOC and NAME are used to locate
 * the object pointed to and the SPACE is used to choose the region pointed to.
 *
 * Return:  Success:    Reference created
 *          Failure:    NULL
 *
 *-------------------------------------------------------------------------
 */
href_t
H5R_create_region(H5G_loc_t *loc, const char *name, H5S_t *space)
{
    H5G_loc_t obj_loc;          /* Group hier. location of object */
    H5G_name_t path;            /* Object group hier. path */
    H5O_loc_t oloc;             /* Object object location */
    hbool_t obj_found = FALSE;  /* Object location found */
    hssize_t buf_size;          /* Size of buffer needed to serialize selection */
    uint8_t *p;                 /* Pointer to OID to store */
    struct href *ref = NULL;         /* Reference to be returned */
    struct href *ret_value = NULL;   /* Return value */

    FUNC_ENTER_PACKAGE_VOL

    HDassert(loc);
    HDassert(name);
    HDassert(space);

    /* Set up object location to fill in */
    obj_loc.oloc = &oloc;
    obj_loc.path = &path;
    H5G_loc_reset(&obj_loc);

    /* Find the object */
    if(H5G_loc_find(loc, name, &obj_loc) < 0)
        HGOTO_ERROR(H5E_REFERENCE, H5E_NOTFOUND, NULL, "object not found")
    obj_found = TRUE;

    /* Get the amount of space required to serialize the selection */
    if((buf_size = H5S_SELECT_SERIAL_SIZE(space)) < 0)
        HGOTO_ERROR(H5E_REFERENCE, H5E_CANTINIT, NULL, "Invalid amount of space for serializing selection")

    /* Increase buffer size to allow for the dataset OID */
    buf_size += (hssize_t)sizeof(haddr_t);

    /* Allocate the space to store the serialized information */
    H5_CHECK_OVERFLOW(buf_size, hssize_t, size_t);
    if(NULL == (ref = (struct href *)H5MM_malloc(sizeof(struct href))))
        HGOTO_ERROR(H5E_RESOURCE, H5E_NOSPACE, NULL, "Cannot allocate memory for reference")
    if(NULL == (ref->ref.serial.buf = H5MM_malloc((size_t) buf_size)))
        HGOTO_ERROR(H5E_REFERENCE, H5E_CANTALLOC, NULL, "Cannot allocate buffer to serialize selection")
    ref->ref.serial.buf_size = (size_t) buf_size;
    ref->ref_type = H5R_REGION;

    /* Serialize information for dataset OID into buffer */
    p = (uint8_t *)ref->ref.serial.buf;
    H5F_addr_encode(loc->oloc->file, &p, obj_loc.oloc->addr);

    /* Serialize the selection into buffer */
    if(H5S_SELECT_SERIALIZE(space, &p) < 0)
        HGOTO_ERROR(H5E_REFERENCE, H5E_CANTCOPY, NULL, "Unable to serialize selection")

    ret_value = ref;

done:
    if(obj_found)
        H5G_loc_free(&obj_loc);
    if(NULL == ret_value) {
        H5MM_free(ref->ref.serial.buf);
        H5MM_free(ref);
    }

    FUNC_LEAVE_NOAPI_VOL(ret_value)
} /* H5R_create_region */

/*-------------------------------------------------------------------------
 * Function:    H5R_create_attr
 *
 * Purpose: Creates an attribute reference. The LOC, NAME and ATTR_NAME are
 * used to locate the attribute pointed to.
 *
 * Return:  Success:    Reference created
 *          Failure:    NULL
 *
 *-------------------------------------------------------------------------
 */
href_t
H5R_create_attr(H5G_loc_t *loc, const char *name, const char *attr_name)
{
    H5G_loc_t obj_loc;          /* Group hier. location of object */
    H5G_name_t path;            /* Object group hier. path */
    H5O_loc_t oloc;             /* Object object location */
    hbool_t obj_found = FALSE;  /* Object location found */
    size_t buf_size;            /* Size of buffer needed to serialize attribute */
    size_t attr_name_len;       /* Length of the attribute name */
    uint8_t *p;                 /* Pointer to OID to store */
    struct href *ref = NULL;         /* Reference to be returned */
    struct href *ret_value = NULL;   /* Return value */

    FUNC_ENTER_PACKAGE_VOL

    HDassert(loc);
    HDassert(name);
    HDassert(attr_name);

    /* Set up object location to fill in */
    obj_loc.oloc = &oloc;
    obj_loc.path = &path;
    H5G_loc_reset(&obj_loc);

    /* Find the object */
    if(H5G_loc_find(loc, name, &obj_loc) < 0)
        HGOTO_ERROR(H5E_REFERENCE, H5E_NOTFOUND, NULL, "object not found")
    obj_found = TRUE;

    /* Get the amount of space required to serialize the attribute name */
    attr_name_len = HDstrlen(attr_name);
    if(attr_name_len >= H5R_MAX_ATTR_REF_NAME_LEN)
        HGOTO_ERROR(H5E_REFERENCE, H5E_ARGS, NULL, "attribute name too long")

    /* Compute buffer size, allow for the attribute name length and object's OID */
    buf_size = attr_name_len + sizeof(uint16_t) + sizeof(haddr_t);

    /* Allocate the space to store the serialized information */
    if(NULL == (ref = (struct href *)H5MM_malloc(sizeof(struct href))))
        HGOTO_ERROR(H5E_RESOURCE, H5E_NOSPACE, NULL, "Cannot allocate memory for reference")
    if (NULL == (ref->ref.serial.buf = H5MM_malloc(buf_size)))
        HGOTO_ERROR(H5E_REFERENCE, H5E_CANTALLOC, NULL, "Cannot allocate buffer to serialize selection")
    ref->ref.serial.buf_size = buf_size;
    ref->ref_type = H5R_ATTR;

    /* Serialize information for object's OID into buffer */
    p = (uint8_t *)ref->ref.serial.buf;
    H5F_addr_encode(loc->oloc->file, &p, obj_loc.oloc->addr);

    /* Serialize information for attribute name length into the buffer */
    UINT16ENCODE(p, attr_name_len);

    /* Copy the attribute name into the buffer */
    HDmemcpy(p, attr_name, attr_name_len);

    /* Sanity check */
    HDassert((size_t)((p + attr_name_len) - (uint8_t *)ref->ref.serial.buf) == buf_size);

    ret_value = ref;

done:
    if(obj_found)
        H5G_loc_free(&obj_loc);
    if(NULL == ret_value) {
        H5MM_free(ref->ref.serial.buf);
        H5MM_free(ref);
    }

    FUNC_LEAVE_NOAPI_VOL(ret_value)
} /* H5R_create_attr */

/*-------------------------------------------------------------------------
 * Function:    H5R_create_ext_object
 *
 * Purpose: Creates an external object reference. The FILENAME and PATHNAME
 * are used to locate the object pointed to.
 *
 * Return:  Success:    Reference created
 *          Failure:    NULL
 *
 *-------------------------------------------------------------------------
 */
href_t
H5R_create_ext_object(const char *filename, const char *pathname)
{
    size_t filename_len;        /* Length of the file name */
    size_t pathname_len;        /* Length of the obj path name */
    size_t buf_size;            /* Size of buffer needed to serialize reference */
    uint8_t *p;                 /* Pointer to OID to store */
    struct href *ref = NULL;         /* Reference to be returned */
    struct href *ret_value = NULL;   /* Return value */

    FUNC_ENTER_PACKAGE_VOL

    HDassert(filename);
    HDassert(pathname);

    /* Need to add name of the file */
    filename_len = HDstrlen(filename);

    /* Need to add object path name */
    pathname_len = HDstrlen(pathname);

    /* Compute buffer size, allow for the file name and object path name lengths */
    buf_size = filename_len + pathname_len + 2 * sizeof(uint16_t);

    if(NULL == (ref = (struct href *)H5MM_malloc(sizeof(struct href))))
        HGOTO_ERROR(H5E_RESOURCE, H5E_NOSPACE, NULL, "Cannot allocate memory for reference")
    if (NULL == (ref->ref.serial.buf = H5MM_malloc(buf_size)))
        HGOTO_ERROR(H5E_REFERENCE, H5E_CANTALLOC, NULL, "Cannot allocate buffer to serialize selection")
    ref->ref.serial.buf_size = buf_size;
    ref->ref_type = H5R_EXT_OBJECT;

    /* Serialize information for file name length into the buffer */
    p = (uint8_t *)ref->ref.serial.buf;
    UINT16ENCODE(p, filename_len);

    /* Copy the file name into the buffer */
    HDmemcpy(p, filename, filename_len);
    p += filename_len;

    /* Serialize information for object path name length into the buffer */
    UINT16ENCODE(p, pathname_len);

    /* Copy the object path name into the buffer */
    HDmemcpy(p, pathname, pathname_len);

    /* Sanity check */
    HDassert((size_t)((p + pathname_len) - (uint8_t *)ref->ref.serial.buf) == buf_size);

    ret_value = ref;

done:
    if(NULL == ret_value) {
        H5MM_free(ref->ref.serial.buf);
        H5MM_free(ref);
    }

    FUNC_LEAVE_NOAPI_VOL(ret_value)
} /* end H5R_create_ext_object() */

/*-------------------------------------------------------------------------
 * Function:    H5R_create_ext_region
 *
 * Purpose: Creates an external region reference. The FILENAME and PATHNAME
 * are used to locate the object pointed to and the SPACE is used to choose
 * the region pointed to.
 *
 * Return:  Success:    Reference created
 *          Failure:    NULL
 *
 *-------------------------------------------------------------------------
 */
href_t
H5R_create_ext_region(const char *filename, const char *pathname, H5S_t *space)
{
    size_t filename_len;        /* Length of the file name */
    size_t pathname_len;        /* Length of the obj path name */
    size_t buf_size;            /* Size of buffer needed to serialize selection */
    size_t space_buf_size;      /* Size of buffer needed to serialize selection */
    uint8_t *p = NULL;          /* Pointer to OID to store */
    struct href *ref = NULL;         /* Reference to be returned */
    struct href *ret_value = NULL;   /* Return value */

    FUNC_ENTER_PACKAGE_VOL

    HDassert(filename);
    HDassert(pathname);
    HDassert(space);

    /* Need to add name of the file */
    filename_len = HDstrlen(filename);

    /* Need to add object path name */
    pathname_len = HDstrlen(pathname);

    /* Get the amount of space required to serialize the selection */
    if (H5S_encode(space, &p, &space_buf_size) < 0)
        HGOTO_ERROR(H5E_REFERENCE, H5E_CANTCOPY, NULL, "Unable to serialize selection")

    /* Compute buffer size, allow for the file name and object path name lengths */
    buf_size = space_buf_size + filename_len + pathname_len + 2 * sizeof(uint16_t);

    /* Allocate the space to store the serialized information */
    if(NULL == (ref = (struct href *)H5MM_malloc(sizeof(struct href))))
        HGOTO_ERROR(H5E_RESOURCE, H5E_NOSPACE, NULL, "Cannot allocate memory for reference")
    if (NULL == (ref->ref.serial.buf = H5MM_malloc((size_t) buf_size)))
        HGOTO_ERROR(H5E_REFERENCE, H5E_CANTALLOC, NULL, "Cannot allocate buffer to serialize selection")
    ref->ref.serial.buf_size = buf_size;
    ref->ref_type = H5R_EXT_REGION;

    /* Serialize information for file name length into the buffer */
    p = (uint8_t *)ref->ref.serial.buf;
    UINT16ENCODE(p, filename_len);

    /* Copy the file name into the buffer */
    HDmemcpy(p, filename, filename_len);
    p += filename_len;

    /* Serialize information for object path name length into the buffer */
    UINT16ENCODE(p, pathname_len);

    /* Copy the object path name into the buffer */
    HDmemcpy(p, pathname, pathname_len);
    p += pathname_len;

    /* Serialize the selection into buffer */
    if (H5S_encode(space, &p, &space_buf_size) < 0)
        HGOTO_ERROR(H5E_REFERENCE, H5E_CANTCOPY, NULL, "Unable to serialize selection")

    /* Sanity check */
    HDassert((size_t)(p - (uint8_t *)ref->ref.serial.buf) == (size_t)buf_size);

    ret_value = ref;

done:
    if(NULL == ret_value) {
        H5MM_free(ref->ref.serial.buf);
        H5MM_free(ref);
    }

    FUNC_LEAVE_NOAPI_VOL(ret_value)
} /* H5R_create_ext_region */

/*-------------------------------------------------------------------------
 * Function:    H5R_create_ext_attr
 *
 * Purpose: Creates an external attribute reference. The FILENAME, PATHNAME
 * and ATTR_NAME are used to locate the attribute pointed to.
 *
 * Return:  Success:    Reference created
 *          Failure:    NULL
 *
 *-------------------------------------------------------------------------
 */
href_t
H5R_create_ext_attr(const char *filename, const char *pathname,
    const char *attr_name)
{
    size_t filename_len;        /* Length of the file name */
    size_t pathname_len;        /* Length of the obj path name */
    size_t attr_name_len;       /* Length of the attribute name */
    size_t buf_size;            /* Size of buffer needed to serialize reference */
    uint8_t *p;                 /* Pointer to OID to store */
    struct href *ref = NULL;         /* Reference to be returned */
    struct href *ret_value = NULL;   /* Return value */

    FUNC_ENTER_PACKAGE_VOL

    HDassert(filename);
    HDassert(pathname);
    HDassert(attr_name);

    /* Need to add name of the file */
    filename_len = HDstrlen(filename);

    /* Need to add object path name */
    pathname_len = HDstrlen(pathname);

    /* Get the amount of space required to serialize the attribute name */
    attr_name_len = HDstrlen(attr_name);
    if(attr_name_len >= H5R_MAX_ATTR_REF_NAME_LEN)
        HGOTO_ERROR(H5E_REFERENCE, H5E_ARGS, NULL, "attribute name too long")

    /* Compute buffer size, allow for the file name and object path name lengths */
    buf_size = filename_len + pathname_len + attr_name_len + 3 * sizeof(uint16_t);

    /* Allocate the space to store the serialized information */
    if(NULL == (ref = (struct href *)H5MM_malloc(sizeof(struct href))))
        HGOTO_ERROR(H5E_RESOURCE, H5E_NOSPACE, NULL, "Cannot allocate memory for reference")
    if (NULL == (ref->ref.serial.buf = H5MM_malloc(buf_size)))
        HGOTO_ERROR(H5E_REFERENCE, H5E_CANTALLOC, NULL, "Cannot allocate buffer to serialize selection")
    ref->ref.serial.buf_size = buf_size;
    ref->ref_type = H5R_EXT_ATTR;

    /* Serialize information for file name length into the buffer */
    p = (uint8_t *)ref->ref.serial.buf;
    UINT16ENCODE(p, filename_len);

    /* Copy the file name into the buffer */
    HDmemcpy(p, filename, filename_len);
    p += filename_len;

    /* Serialize information for object path name length into the buffer */
    UINT16ENCODE(p, pathname_len);

    /* Copy the object path name into the buffer */
    HDmemcpy(p, pathname, pathname_len);
    p += pathname_len;

    /* Serialize information for attribute name length into the buffer */
    UINT16ENCODE(p, attr_name_len);

    /* Copy the attribute name into the buffer */
    HDmemcpy(p, attr_name, attr_name_len);

    /* Sanity check */
    HDassert((size_t)((p + attr_name_len) - (uint8_t *)ref->ref.serial.buf) == buf_size);

    ret_value = ref;

done:
    if(NULL == ret_value) {
        H5MM_free(ref->ref.serial.buf);
        H5MM_free(ref);
    }

    FUNC_LEAVE_NOAPI_VOL(ret_value)
} /* H5R_create_ext_attr */

/*-------------------------------------------------------------------------
 * Function:    H5R_destroy
 *
 * Purpose: Destroy reference and free resources allocated during H5R_create.
 *
 * Return:  Non-negative on success/Negative on failure
 *
 *-------------------------------------------------------------------------
 */
herr_t
H5R_destroy(href_t _ref)
{
    struct href *ref = (struct href *) _ref; /* Reference */
    herr_t    ret_value = SUCCEED; /* Return value */

    FUNC_ENTER_NOAPI_NOINIT_NOERR

    HDassert(ref);

    if(H5R_OBJECT != ref->ref_type)
        H5MM_free(ref->ref.serial.buf);
    H5MM_free(ref);

    FUNC_LEAVE_NOAPI(ret_value)
} /* end H5R_destroy() */

/*-------------------------------------------------------------------------
 * Function:    H5R_get_type
 *
 * Purpose: Given a reference to some object, return the type of that reference.
 *
 * Return:  Type of the reference
 *
 *-------------------------------------------------------------------------
 */
H5R_type_t
H5R_get_type(href_t _ref)
{
    struct href *ref = (struct href *) _ref; /* Reference */
    H5R_type_t ret_value = H5R_BADTYPE;

    FUNC_ENTER_NOAPI_NOINIT_NOERR

    HDassert(ref);
    ret_value = ref->ref_type;

    FUNC_LEAVE_NOAPI(ret_value)
} /* end H5R_get_type() */

/*-------------------------------------------------------------------------
 * Function:    H5R__equal
 *
 * Purpose: Compare two references
 *
 * Return:  TRUE if equal, FALSE if unequal, FAIL if error
 *
 *-------------------------------------------------------------------------
 */
htri_t
H5R__equal(href_t _ref1, href_t _ref2)
{
    struct href *ref1 = (struct href *) _ref1; /* Reference */
    struct href *ref2 = (struct href *) _ref2; /* Reference */
    htri_t ret_value = FAIL;

    FUNC_ENTER_NOAPI_NOINIT

    HDassert(ref1);
    HDassert(ref2);

    if (ref1->ref_type != ref2->ref_type)
        HGOTO_DONE(FALSE);

    switch (ref1->ref_type) {
        case H5R_OBJECT:
            if (ref1->ref.addr != ref2->ref.addr)
                HGOTO_DONE(FALSE);
            break;
        case H5R_REGION:
        case H5R_ATTR:
        case H5R_EXT_OBJECT:
        case H5R_EXT_REGION:
        case H5R_EXT_ATTR:
            if (ref1->ref.serial.buf_size != ref2->ref.serial.buf_size)
                HGOTO_DONE(FALSE);
            if (0 != HDmemcmp(ref1->ref.serial.buf, ref2->ref.serial.buf, ref1->ref.serial.buf_size))
                HGOTO_DONE(FALSE);
            break;
        default:
            HDassert("unknown reference type" && 0);
            HGOTO_ERROR(H5E_REFERENCE, H5E_UNSUPPORTED, FAIL, "internal error (unknown reference type)")
    }

    ret_value = TRUE;

done:
    FUNC_LEAVE_NOAPI(ret_value)
} /* end H5R__equal() */

/*-------------------------------------------------------------------------
 * Function:    H5R__copy
 *
 * Purpose: Copy a reference
 *
 * Return:  Success:    Reference created
 *          Failure:    NULL
 *
 *-------------------------------------------------------------------------
 */
href_t
H5R__copy(href_t _ref)
{
    struct href *ref = (struct href *) _ref; /* Reference */
    struct href *new_ref = NULL;
    href_t ret_value = NULL;

    FUNC_ENTER_NOAPI_NOINIT

    HDassert(ref);

    /* Allocate the space to store the serialized information */
    if(NULL == (new_ref = (struct href *)H5MM_malloc(sizeof(struct href))))
        HGOTO_ERROR(H5E_RESOURCE, H5E_NOSPACE, NULL, "Cannot allocate memory for reference")
    new_ref->ref_type = ref->ref_type;

    switch (ref->ref_type) {
        case H5R_OBJECT:
            new_ref->ref.addr = ref->ref.addr;
            break;
        case H5R_REGION:
        case H5R_ATTR:
        case H5R_EXT_OBJECT:
        case H5R_EXT_REGION:
        case H5R_EXT_ATTR:
            if (0 == (new_ref->ref.serial.buf_size = ref->ref.serial.buf_size))
                HGOTO_ERROR(H5E_REFERENCE, H5E_BADVALUE, NULL, "Invalid reference buffer size")
            if (NULL == (new_ref->ref.serial.buf = H5MM_malloc(new_ref->ref.serial.buf_size)))
                HGOTO_ERROR(H5E_REFERENCE, H5E_CANTALLOC, NULL, "Cannot allocate reference buffer")
            HDmemcpy(new_ref->ref.serial.buf, ref->ref.serial.buf, ref->ref.serial.buf_size);
            break;
        default:
            HDassert("unknown reference type" && 0);
            HGOTO_ERROR(H5E_REFERENCE, H5E_UNSUPPORTED, NULL, "internal error (unknown reference type)")
    }

    ret_value = new_ref;

done:
    FUNC_LEAVE_NOAPI(ret_value)
} /* end H5R__copy() */

/*-------------------------------------------------------------------------
 * Function:    H5R__get_object
 *
 * Purpose: Given a reference to some object, open that object and return an
 * ID for that object.
 *
 * Return:  Valid ID on success/Negative on failure
 *
 *-------------------------------------------------------------------------
 */
hid_t
H5R__get_object(H5F_t *file, hid_t oapl_id, href_t _ref, hbool_t app_ref)
{
    H5O_loc_t oloc;                 /* Object location */
    H5G_name_t path;                /* Path of object */
    H5G_loc_t loc;                  /* Group location */
    char *pathname = NULL;          /* Path name of the object (for external references) */
    unsigned rc;                    /* Reference count of object */
    H5O_type_t obj_type;            /* Type of object */
    hbool_t ext_ref = FALSE;        /* External reference */
    const uint8_t *p = NULL;        /* Pointer to OID to store */
    struct href *ref = (struct href *) _ref; /* Reference */
    hid_t ret_value = H5I_BADID;    /* Return value */

    FUNC_ENTER_NOAPI_NOINIT

    HDassert(file);
    HDassert(ref);
    HDassert(ref->ref_type > H5R_BADTYPE && ref->ref_type < H5R_MAXTYPE);

    /* Initialize the object location */
    H5O_loc_reset(&oloc);
    oloc.file = file;

    /* Construct a group location for opening the object */
    H5G_name_reset(&path);
    loc.oloc = &oloc;
    loc.path = &path;

    /* Point to reference buffer now */
    p = (const uint8_t *)ref->ref.serial.buf;

    /* Decode reference */
    switch (ref->ref_type) {
        case H5R_OBJECT:
            oloc.addr = ref->ref.addr;
            if(!H5F_addr_defined(oloc.addr) || oloc.addr == 0)
                HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, FAIL, "Undefined reference pointer")
            break;
        case H5R_REGION:
        case H5R_ATTR:
            /* Get the object oid for the dataset */
            H5F_addr_decode(oloc.file, &p, &(oloc.addr));
            break;
        case H5R_EXT_OBJECT:
        case H5R_EXT_REGION:
        case H5R_EXT_ATTR:
        {
            size_t filename_len; /* Length of the file name */
            size_t pathname_len; /* Length of the obj path name */

            /* Get the file name length */
            UINT16DECODE(p, filename_len);
            HDassert(filename_len < H5R_MAX_ATTR_REF_NAME_LEN);

            /* Skip the file name */
            p += filename_len;

            /* Get the path name length */
            UINT16DECODE(p, pathname_len);

            /* Allocate space for the path name */
            if(NULL == (pathname = (char *)H5MM_malloc(pathname_len + 1)))
                HGOTO_ERROR(H5E_REFERENCE, H5E_CANTALLOC, FAIL, "memory allocation failed")

            /* Get the path name */
            HDmemcpy(pathname, p, pathname_len);
            pathname[pathname_len] = '\0';

            /* Skip the path name */
            p += pathname_len;

            /* Mark as external reference */
            ext_ref = TRUE;
        }
            break;
        case H5R_BADTYPE:
        case H5R_MAXTYPE:
        default:
            HDassert("unknown reference type" && 0);
            HGOTO_ERROR(H5E_REFERENCE, H5E_UNSUPPORTED, FAIL, "internal error (unknown reference type)")
    }

    /* Get object location */
    if (ext_ref) {
        H5G_loc_t file_loc;  /* File root group location */

        /* Construct a group location for root group of the file */
        if (FAIL == H5G_root_loc(file, &file_loc))
            HGOTO_ERROR(H5E_SYM, H5E_BADVALUE, FAIL, "unable to create location for file")

        /* Find the object's location */
        if (H5G_loc_find(&file_loc, pathname, &loc/*out*/) < 0)
            HGOTO_ERROR(H5E_OHDR, H5E_NOTFOUND, FAIL, "object not found")

        /* Find the object's type */
        if (H5O_obj_type(loc.oloc, &obj_type) < 0)
            HGOTO_ERROR(H5E_OHDR, H5E_CANTGET, FAIL, "cannot get object type")
    } else {
        /* Get the # of links for object, and its type */
        /* (To check to make certain that this object hasn't been deleted since the reference was created) */
        if(H5O_get_rc_and_type(&oloc, &rc, &obj_type) < 0 || 0 == rc)
            HGOTO_ERROR(H5E_REFERENCE, H5E_LINKCOUNT, FAIL, "dereferencing deleted object")
    }

    /* Open the object */
    switch(obj_type) {
        case H5O_TYPE_GROUP:
        {
            H5G_t *group;               /* Pointer to group to open */

            if(NULL == (group = H5G_open(&loc)))
                HGOTO_ERROR(H5E_SYM, H5E_NOTFOUND, FAIL, "not found")

            /* Create an atom for the group */
            if((ret_value = H5I_register(H5I_GROUP, group, app_ref)) < 0) {
                H5G_close(group);
                HGOTO_ERROR(H5E_SYM, H5E_CANTREGISTER, FAIL, "can't register group")
            } /* end if */
        } /* end case */
        break;

        case H5O_TYPE_NAMED_DATATYPE:
        {
            H5T_t *type;                /* Pointer to datatype to open */

            if(NULL == (type = H5T_open(&loc)))
                HGOTO_ERROR(H5E_DATATYPE, H5E_NOTFOUND, FAIL, "not found")

            /* Create an atom for the datatype */
            if((ret_value = H5I_register(H5I_DATATYPE, type, app_ref)) < 0) {
                H5T_close(type);
                HGOTO_ERROR(H5E_DATATYPE, H5E_CANTREGISTER, FAIL, "can't register datatype")
            } /* end if */
        } /* end case */
        break;

        case H5O_TYPE_DATASET:
        {
            H5D_t *dset;                /* Pointer to dataset to open */

            /* Get correct property list */
            if(H5P_DEFAULT == oapl_id)
                oapl_id = H5P_DATASET_ACCESS_DEFAULT;
            else if(TRUE != H5P_isa_class(oapl_id, H5P_DATASET_ACCESS))
                HGOTO_ERROR(H5E_ARGS, H5E_BADTYPE, FAIL, "not dataset access property list")

            /* Open the dataset */
            if(NULL == (dset = H5D_open(&loc, oapl_id)))
                HGOTO_ERROR(H5E_DATASET, H5E_NOTFOUND, FAIL, "not found")

            /* Create an atom for the dataset */
            if((ret_value = H5I_register(H5I_DATASET, dset, app_ref)) < 0) {
                H5D_close(dset);
                HGOTO_ERROR(H5E_DATASET, H5E_CANTREGISTER, FAIL, "can't register dataset")
            } /* end if */
        } /* end case */
        break;

        case H5O_TYPE_UNKNOWN:
        case H5O_TYPE_NTYPES:
        default:
            HGOTO_ERROR(H5E_REFERENCE, H5E_BADTYPE, FAIL, "can't identify type of object referenced")
    } /* end switch */

done:
    H5MM_xfree(pathname);

    FUNC_LEAVE_NOAPI(ret_value)
} /* end H5R__get_object() */

/*-------------------------------------------------------------------------
 * Function:    H5R__get_region
 *
 * Purpose: Given a reference to some object, creates a copy of the dataset
 * pointed to's dataspace and defines a selection in the copy which is the
 * region pointed to.
 *
 * Return:  Pointer to the dataspace on success/NULL on failure
 *
 *-------------------------------------------------------------------------
 */
H5S_t *
H5R__get_region(H5F_t *file, href_t _ref)
{
    const uint8_t *p = NULL;    /* Pointer to OID to store */
    struct href *ref = (struct href *) _ref; /* Reference */
    H5S_t *ret_value = NULL;

    FUNC_ENTER_NOAPI_NOINIT

    HDassert(ref);
    HDassert((ref->ref_type == H5R_REGION) || (ref->ref_type == H5R_EXT_REGION));

    /* Point to reference buffer now */
    p = (const uint8_t *)ref->ref.serial.buf;

    if (ref->ref_type == H5R_EXT_REGION) {
        size_t filename_len; /* Length of the file name */
        size_t pathname_len; /* Length of the obj path name */

        /* Get the file name length */
        UINT16DECODE(p, filename_len);
        HDassert(filename_len < H5R_MAX_ATTR_REF_NAME_LEN);

        /* Skip the file name */
        p += filename_len;

        /* Get the path name length */
        UINT16DECODE(p, pathname_len);

        /* Skip the path name */
        p += pathname_len;

        /* Unserialize the selection */
        if (NULL == (ret_value = H5S_decode(&p)))
            HGOTO_ERROR(H5E_REFERENCE, H5E_CANTDECODE, NULL, "can't deserialize selection")
    } else {
        H5O_loc_t oloc; /* Object location */

        HDassert(file);

        /* Initialize the object location */
        H5O_loc_reset(&oloc);
        oloc.file = file;

        /* Get the object oid for the dataset */
        H5F_addr_decode(oloc.file, &p, &(oloc.addr));

        /* Open and copy the dataset's dataspace */
        if((ret_value = H5S_read(&oloc)) == NULL)
            HGOTO_ERROR(H5E_DATASPACE, H5E_NOTFOUND, NULL, "not found")

        /* Unserialize the selection */
        if(H5S_SELECT_DESERIALIZE(&ret_value, &p) < 0)
            HGOTO_ERROR(H5E_REFERENCE, H5E_CANTDECODE, NULL, "can't deserialize selection")
    }

done:
    FUNC_LEAVE_NOAPI(ret_value)
} /* end H5R__get_region() */

/*-------------------------------------------------------------------------
 * Function:    H5R__get_attr
 *
 * Purpose: Given a reference to some attribute, open that attribute and
 * return an ID for that attribute.
 *
 * Return:  Valid ID on success/Negative on failure
 *
 *-------------------------------------------------------------------------
 */
struct H5A_t *
H5R__get_attr(H5F_t *file, href_t _ref)
{
    H5O_loc_t oloc;                 /* Object location */
    H5G_name_t path;                /* Path of object */
    H5G_loc_t loc;                  /* Group location */
    char *pathname = NULL;          /* Path name of the object (for external references) */
    size_t attr_name_len;           /* Length of the attribute name */
    char *attr_name = NULL;         /* Attribute name */
    const uint8_t *p = NULL;        /* Pointer to OID to store */
    struct href *ref = (struct href *) _ref; /* Reference */
    struct H5A_t *ret_value = NULL; /* Return value */

    FUNC_ENTER_NOAPI_NOINIT

    HDassert(file);
    HDassert(ref);
    HDassert((ref->ref_type == H5R_ATTR) || (ref->ref_type == H5R_EXT_ATTR));

    /* Initialize the object location */
    H5O_loc_reset(&oloc);
    oloc.file = file;

    /* Construct a group location for opening the object */
    H5G_name_reset(&path);
    loc.oloc = &oloc;
    loc.path = &path;

    /* Point to reference buffer now */
    p = (const uint8_t *)ref->ref.serial.buf;

    if (ref->ref_type == H5R_EXT_ATTR) {
        H5G_loc_t file_loc;  /* File root group location */
        size_t filename_len; /* Length of the file name */
        size_t pathname_len; /* Length of the obj path name */

        /* Get the file name length */
        UINT16DECODE(p, filename_len);

        /* Skip the file name */
        p += filename_len;

        /* Get the path name length */
        UINT16DECODE(p, pathname_len);

        /* Allocate space for the path name */
        if(NULL == (pathname = (char *)H5MM_malloc(pathname_len + 1)))
            HGOTO_ERROR(H5E_REFERENCE, H5E_CANTALLOC, NULL, "memory allocation failed")

        /* Get the path name */
        HDmemcpy(pathname, p, pathname_len);
        pathname[pathname_len] = '\0';

        /* Skip the path name */
        p += pathname_len;

        /* Construct a group location for root group of the file */
        if (FAIL == H5G_root_loc(file, &file_loc))
            HGOTO_ERROR(H5E_SYM, H5E_BADVALUE, NULL, "unable to create location for file")

        /* Find the object's location */
        if (H5G_loc_find(&file_loc, pathname, &loc/*out*/) < 0)
            HGOTO_ERROR(H5E_OHDR, H5E_NOTFOUND, NULL, "object not found")
    } else {
        /* Get the object oid for the dataset */
        H5F_addr_decode(oloc.file, &p, &(oloc.addr));
    }

    /* Get the attribute name length */
    UINT16DECODE(p, attr_name_len);
    HDassert(attr_name_len < H5R_MAX_ATTR_REF_NAME_LEN);

    /* Allocate space for the attribute name */
    if(NULL == (attr_name = (char *)H5MM_malloc(attr_name_len + 1)))
        HGOTO_ERROR(H5E_REFERENCE, H5E_CANTALLOC, NULL, "memory allocation failed")

    /* Get the attribute name */
    HDmemcpy(attr_name, p, attr_name_len);
    attr_name[attr_name_len] = '\0';

    /* Open the attribute */
    if(NULL == (ret_value = H5A_open(&loc, attr_name)))
        HGOTO_ERROR(H5E_REFERENCE, H5E_CANTOPENOBJ, NULL, "can't open attribute")

done:
    H5MM_xfree(pathname);
    H5MM_xfree(attr_name);

    FUNC_LEAVE_NOAPI(ret_value)
} /* end H5R__get_attr() */

/*-------------------------------------------------------------------------
 * Function:    H5R__get_obj_type
 *
 * Purpose: Given a reference to some object, this function returns the type
 * of object pointed to.
 *
 * Return:  Non-negative on success/Negative on failure
 *
 *-------------------------------------------------------------------------
 */
herr_t
H5R__get_obj_type(H5F_t *file, href_t _ref, H5O_type_t *obj_type)
{
    H5O_loc_t oloc;                 /* Object location */
    H5G_name_t path;                /* Path of object */
    H5G_loc_t loc;                  /* Group location */
    char *pathname = NULL;          /* Path name of the object (for external references) */
    unsigned rc;                    /* Reference count of object */
    hbool_t ext_ref = FALSE;        /* External reference */
    const uint8_t *p = NULL;        /* Pointer to OID to store */
    struct href *ref = (struct href *) _ref; /* Reference */
    herr_t ret_value = SUCCEED;     /* Return value */

    FUNC_ENTER_NOAPI_NOINIT

    HDassert(file);
    HDassert(ref);
    HDassert(ref->ref_type > H5R_BADTYPE && ref->ref_type < H5R_MAXTYPE);
    HDassert(obj_type);

    /* Initialize the object location */
    H5O_loc_reset(&oloc);
    oloc.file = file;

    /* Construct a group location for opening the object */
    H5G_name_reset(&path);
    loc.oloc = &oloc;
    loc.path = &path;

    /* Point to reference buffer now */
    p = (const uint8_t *)ref->ref.serial.buf;

    switch(ref->ref_type) {
        case H5R_OBJECT:
            /* Get the object oid */
            oloc.addr = ref->ref.addr;
            break;

        case H5R_REGION:
        case H5R_ATTR:
            /* Get the object oid for the dataset */
            H5F_addr_decode(oloc.file, &p, &(oloc.addr));
            break;

        case H5R_EXT_OBJECT:
        case H5R_EXT_REGION:
        case H5R_EXT_ATTR:
        {
            size_t filename_len; /* Length of the file name */
            size_t pathname_len; /* Length of the obj path name */

            /* Get the file name length */
            UINT16DECODE(p, filename_len);
            HDassert(filename_len < H5R_MAX_ATTR_REF_NAME_LEN);

            /* Skip the file name */
            p += filename_len;

            /* Get the path name length */
            UINT16DECODE(p, pathname_len);

            /* Allocate space for the path name */
            if(NULL == (pathname = (char *)H5MM_malloc(pathname_len + 1)))
                HGOTO_ERROR(H5E_REFERENCE, H5E_CANTALLOC, FAIL, "memory allocation failed")

            /* Get the path name */
            HDmemcpy(pathname, p, pathname_len);
            pathname[pathname_len] = '\0';

            /* Skip the path name */
            p += pathname_len;

            /* Mark as external reference */
            ext_ref = TRUE;
            break;
        } /* end case */

        case H5R_BADTYPE:
        case H5R_MAXTYPE:
        default:
            HDassert("unknown reference type" && 0);
            HGOTO_ERROR(H5E_REFERENCE, H5E_UNSUPPORTED, FAIL, "internal error (unknown reference type)")
    } /* end switch */

    if (ext_ref) {
        H5G_loc_t file_loc;  /* File root group location */

        /* Construct a group location for root group of the file */
        if (FAIL == H5G_root_loc(file, &file_loc))
            HGOTO_ERROR(H5E_SYM, H5E_BADVALUE, FAIL, "unable to create location for file")

        /* Find the object's location */
        if (H5G_loc_find(&file_loc, pathname, &loc/*out*/) < 0)
            HGOTO_ERROR(H5E_OHDR, H5E_NOTFOUND, FAIL, "object not found")

        /* Find the object's type */
        if (H5O_obj_type(loc.oloc, obj_type) < 0)
            HGOTO_ERROR(H5E_OHDR, H5E_CANTGET, FAIL, "cannot get object type")
    } else {
        /* Get the # of links for object, and its type */
        /* (To check to make certain that this object hasn't been deleted since the reference was created) */
        if(H5O_get_rc_and_type(&oloc, &rc, obj_type) < 0 || 0 == rc)
            HGOTO_ERROR(H5E_REFERENCE, H5E_LINKCOUNT, FAIL, "dereferencing deleted object")
    }

done:
    H5MM_xfree(pathname);

    FUNC_LEAVE_NOAPI(ret_value)
} /* end H5R__get_obj_type() */

/*-------------------------------------------------------------------------
 * Function:    H5R__get_obj_name
 *
 * Purpose: Given a reference to some object, determine a path to the object
 * referenced in the file.
 *
 * Return:  Non-negative length of the path on success/Negative on failure
 *
 *-------------------------------------------------------------------------
 */
ssize_t
H5R__get_obj_name(H5F_t *f, hid_t lapl_id, href_t _ref, char *name, size_t size)
{
    hid_t file_id = H5I_BADID;  /* ID for file that the reference is in */
    H5O_loc_t oloc;             /* Object location describing object for reference */
    ssize_t ret_value = -1;     /* Return value */
    hbool_t ext_ref = FALSE;    /* External reference */
    const uint8_t *p = NULL;    /* Pointer to reference to decode */
    struct href *ref = (struct href *) _ref; /* Reference */

    FUNC_ENTER_NOAPI_NOINIT

    /* Check args */
    HDassert(ref);

    /* Point to reference buffer now */
    p = (const uint8_t *)ref->ref.serial.buf;

    /* Get address for reference */
    switch(ref->ref_type) {
        case H5R_OBJECT:
        {
            HDassert(f);

            /* Initialize the object location */
            H5O_loc_reset(&oloc);
            oloc.file = f;

            oloc.addr = ref->ref.addr;
        }
            break;

        case H5R_REGION:
        case H5R_ATTR:
        {
            HDassert(f);

            /* Initialize the object location */
            H5O_loc_reset(&oloc);
            oloc.file = f;

            /* Get the object oid for the dataset */
            H5F_addr_decode(oloc.file, &p, &(oloc.addr));
        } /* end case */
        break;

        case H5R_EXT_OBJECT:
        case H5R_EXT_REGION:
        case H5R_EXT_ATTR:
        {
            size_t filename_len; /* Length of the file name */
            size_t pathname_len; /* Length of the obj path name */
            size_t copy_len;

            /* Get the file name length */
            UINT16DECODE(p, filename_len);
            HDassert(filename_len < H5R_MAX_ATTR_REF_NAME_LEN);

            /* Skip the file name */
            p += filename_len;

            /* Get the path name length */
            UINT16DECODE(p, pathname_len);
            copy_len = pathname_len;

            /* Get the path name */
            if (name) {
                copy_len = MIN(copy_len, size - 1);
                HDmemcpy(name, p, copy_len);
                name[copy_len] = '\0';
            }

            ret_value = (ssize_t)(copy_len + 1);

            /* Mark as external reference */
            ext_ref = TRUE;
        }
            break;

        case H5R_BADTYPE:
        case H5R_MAXTYPE:
        default:
            HDassert("unknown reference type" && 0);
            HGOTO_ERROR(H5E_REFERENCE, H5E_UNSUPPORTED, FAIL, "internal error (unknown reference type)")
    } /* end switch */

    /* Get object location */
    if (!ext_ref) {
        /* Retrieve file ID for name search */
        if((file_id = H5F_get_id(f, FALSE)) < 0)
            HGOTO_ERROR(H5E_ATOM, H5E_CANTGET, (-1), "can't get file ID")

        /* Get name, length, etc. */
        if((ret_value = H5G_get_name_by_addr(file_id, &oloc, name, size)) < 0)
            HGOTO_ERROR(H5E_REFERENCE, H5E_CANTGET, FAIL, "can't determine name")
    }

done:
    /* Close file ID used for search */
    if(file_id > 0 && H5I_dec_ref(file_id) < 0)
        HDONE_ERROR(H5E_REFERENCE, H5E_CANTDEC, FAIL, "can't decrement ref count of temp ID")
    FUNC_LEAVE_NOAPI(ret_value)
} /* end H5R__get_obj_name() */

/*-------------------------------------------------------------------------
 * Function:    H5R__get_attr_name
 *
 * Purpose: Given a reference to some attribute, determine its name.
 *
 * Return:  Non-negative length of the path on success/Negative on failure
 *
 *-------------------------------------------------------------------------
 */
ssize_t
H5R__get_attr_name(H5F_t *f, href_t _ref, char *name, size_t size)
{
    ssize_t ret_value = -1;     /* Return value */
    const uint8_t *p = NULL;    /* Pointer to reference to decode */
    size_t attr_name_len;       /* Length of the attribute name */
    size_t copy_len;
    struct href *ref = (struct href *) _ref; /* Reference */

    FUNC_ENTER_NOAPI_NOINIT_NOERR

    /* Check args */
    HDassert(ref);
    HDassert((ref->ref_type == H5R_ATTR) || (ref->ref_type == H5R_EXT_ATTR));

    /* Point to reference buffer now */
    p = (const uint8_t *)ref->ref.serial.buf;

    if (ref->ref_type == H5R_EXT_ATTR) {
        size_t filename_len; /* Length of the file name */
        size_t pathname_len; /* Length of the obj path name */

        /* Get the file name length */
        UINT16DECODE(p, filename_len);
        HDassert(filename_len < H5R_MAX_ATTR_REF_NAME_LEN);

        /* Skip the file name */
        p += filename_len;

        /* Get the path name length */
        UINT16DECODE(p, pathname_len);

        /* Skip the path name */
        p += pathname_len;
    } else {
        H5O_loc_t oloc; /* Object location describing object for reference */

        HDassert(f);

        /* Initialize the object location */
        H5O_loc_reset(&oloc);
        oloc.file = f;

        /* Get the object oid for the dataset */
        H5F_addr_decode(oloc.file, &p, &(oloc.addr));
    }

    /* Get the attribute name length */
    UINT16DECODE(p, attr_name_len);
    HDassert(attr_name_len < H5R_MAX_ATTR_REF_NAME_LEN);
    copy_len = attr_name_len;

    /* Get the attribute name */
    if (name) {
        copy_len = MIN(copy_len, size - 1);
        HDmemcpy(name, p, copy_len);
        name[copy_len] = '\0';
    }

    ret_value = (ssize_t)(copy_len + 1);

    FUNC_LEAVE_NOAPI(ret_value)
} /* end H5R__get_attr_name() */

/*-------------------------------------------------------------------------
 * Function:    H5R__get_file_name
 *
 * Purpose: Given a reference to some object, determine a file name of the
 * object located into.
 *
 * Return:  Non-negative length of the path on success/Negative on failure
 *
 *-------------------------------------------------------------------------
 */
ssize_t
H5R__get_file_name(href_t _ref, char *name, size_t size)
{
    struct href *ref = (struct href *) _ref; /* Reference */
    ssize_t ret_value = -1;     /* Return value */

    FUNC_ENTER_NOAPI_NOINIT

    /* Check args */
    HDassert(ref);

    switch (ref->ref_type) {
        case H5R_EXT_OBJECT:
        case H5R_EXT_REGION:
        case H5R_EXT_ATTR:
        {
            const uint8_t *p;    /* Pointer to reference to decode */
            size_t filename_len; /* Length of the file name */
            size_t copy_len;

            /* Get the file name length */
            p = (const uint8_t *)ref->ref.serial.buf;
            UINT16DECODE(p, filename_len);
            copy_len = filename_len;

            /* Get the attribute name */
            if (name) {
                copy_len = MIN(copy_len, size - 1);
                HDmemcpy(name, p, copy_len);
                name[copy_len] = '\0';
            }
            ret_value = (ssize_t)(copy_len + 1);
        }
            break;
        case H5R_OBJECT:
        case H5R_REGION:
        case H5R_ATTR:
        case H5R_BADTYPE:
        case H5R_MAXTYPE:
        default:
            HDassert("unknown reference type" && 0);
            HGOTO_ERROR(H5E_REFERENCE, H5E_UNSUPPORTED, FAIL, "internal error (unknown reference type)")
    } /* end switch */

done:
    FUNC_LEAVE_NOAPI(ret_value)
} /* end H5R__get_file_name() */

/*-------------------------------------------------------------------------
 * Function:    H5R_encode
 *
 * Purpose: Private function for H5Rencode.
 *
 * Return:  Non-negative on success/Negative on failure
 *
 *-------------------------------------------------------------------------
 */
herr_t
H5R_encode(href_t _ref, unsigned char *buf, size_t *nalloc)
{
    struct href *ref = (struct href *) _ref; /* Reference */
    size_t ref_buf_size, buf_size;
    herr_t ret_value = SUCCEED;

    FUNC_ENTER_NOAPI_NOINIT_NOERR

    HDassert(ref);
    HDassert(nalloc);

    ref_buf_size = (ref->ref_type == H5R_OBJECT) ? sizeof(ref->ref.addr) : ref->ref.serial.buf_size;
    buf_size = ref_buf_size + sizeof(uint64_t) + 1;

    /* Don't encode if buffer size isn't big enough or buffer is empty */
    if(buf && *nalloc >= buf_size) {
        const void *ref_buf = (ref->ref_type == H5R_OBJECT) ? &ref->ref.addr : ref->ref.serial.buf;

        /* Encode the type of the information */
        *buf++ = ref->ref_type;

        /* Encode buffer size */
        UINT64ENCODE(buf, ref_buf_size);

        /* Encode into user's buffer */
        HDmemcpy(buf, ref_buf, ref_buf_size);
    } /* end if */

    *nalloc = buf_size;

    FUNC_LEAVE_NOAPI(ret_value)
} /* end H5R_encode() */

/*-------------------------------------------------------------------------
 * Function:    H5R_decode
 *
 * Purpose: Private function for H5Rdecode.
 *
 * Return:  Success:    Pointer to the decoded reference
 *          Failure:    NULL
 *
 *-------------------------------------------------------------------------
 */
href_t
H5R_decode(const unsigned char *buf, size_t *_nbytes)
{
    struct href *ret_value = NULL;
    size_t buf_size;
    size_t nbytes = 0;

    FUNC_ENTER_NOAPI_NOINIT

    HDassert(buf);

    if(NULL == (ret_value = (struct href *)H5MM_malloc(sizeof(struct href))))
        HGOTO_ERROR(H5E_RESOURCE, H5E_NOSPACE, NULL, "Cannot allocate memory for reference")

    ret_value->ref_type = (H5R_type_t)*buf++;
    nbytes++;
    if(ret_value->ref_type <= H5R_BADTYPE || ret_value->ref_type >= H5R_MAXTYPE)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, NULL, "invalid reference type");

    UINT64DECODE(buf, buf_size);
    nbytes += sizeof(uint64_t);
    if (!buf_size)
        HGOTO_ERROR(H5E_ARGS, H5E_BADVALUE, NULL, "invalid reference size");
    if (ret_value->ref_type == H5R_OBJECT)
        HDmemcpy(ret_value->ref.addr, buf, buf_size);
    else {
        ret_value->ref.serial.buf_size = buf_size;
        if(NULL == (ret_value->ref.serial.buf = H5MM_malloc(buf_size)))
            HGOTO_ERROR(H5E_RESOURCE, H5E_NOSPACE, NULL, "Cannot allocate memory for reference")
        HDmemcpy(ret_value->ref.serial.buf, buf, buf_size);
    }
    nbytes += buf_size;

    if (_nbytes) *_nbytes = nbytes;

done:
    FUNC_LEAVE_NOAPI(ret_value)
} /* end H5R_decode() */

/*-------------------------------------------------------------------------
 * Function:    H5R_cast
 *
 * Purpose: Cast reference to parent type.
 *
 * Return:  Non-negative on success/Negative on failure
 *
 *-------------------------------------------------------------------------
 */
herr_t
H5R_cast(href_t _ref, H5R_type_t ref_type)
{
    struct href *ref = (struct href *)_ref;
    herr_t ret_value = SUCCEED;

    FUNC_ENTER_NOAPI_NOINIT

    HDassert(ref);

    switch (ref->ref_type) {
        case H5R_EXT_REGION:
        case H5R_EXT_ATTR:
            if (ref_type < ref->ref_type && ref_type > H5R_ATTR)
                /* Safe to cast */
                ref->ref_type = ref_type;
            break;
        case H5R_EXT_OBJECT:
        case H5R_OBJECT:
        case H5R_REGION:
        case H5R_ATTR:
        default:
            HDassert("unknown reference type" && 0);
            HGOTO_ERROR(H5E_REFERENCE, H5E_UNSUPPORTED, FAIL, "internal error (unknown reference type)")
    }

done:
    FUNC_LEAVE_NOAPI(ret_value)
} /* end H5R_cast() */
