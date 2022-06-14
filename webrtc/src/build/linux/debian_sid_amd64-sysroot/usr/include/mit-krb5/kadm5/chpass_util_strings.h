/*
 * et-h-chpass_util_strings.h:
 * This file is automatically generated; please do not edit it.
 */

#include <et/com_err.h>

#define CHPASS_UTIL_GET_POLICY_INFO              (-1492553984L)
#define CHPASS_UTIL_GET_PRINC_INFO               (-1492553983L)
#define CHPASS_UTIL_NEW_PASSWORD_MISMATCH        (-1492553982L)
#define CHPASS_UTIL_NEW_PASSWORD_PROMPT          (-1492553981L)
#define CHPASS_UTIL_NEW_PASSWORD_AGAIN_PROMPT    (-1492553980L)
#define CHPASS_UTIL_NO_PASSWORD_READ             (-1492553979L)
#define CHPASS_UTIL_NO_POLICY_YET_Q_ERROR        (-1492553978L)
#define CHPASS_UTIL_PASSWORD_CHANGED             (-1492553977L)
#define CHPASS_UTIL_PASSWORD_IN_DICTIONARY       (-1492553976L)
#define CHPASS_UTIL_PASSWORD_NOT_CHANGED         (-1492553975L)
#define CHPASS_UTIL_PASSWORD_TOO_SHORT           (-1492553974L)
#define CHPASS_UTIL_TOO_FEW_CLASSES              (-1492553973L)
#define CHPASS_UTIL_PASSWORD_TOO_SOON            (-1492553972L)
#define CHPASS_UTIL_PASSWORD_REUSE               (-1492553971L)
#define CHPASS_UTIL_WHILE_TRYING_TO_CHANGE       (-1492553970L)
#define CHPASS_UTIL_WHILE_READING_PASSWORD       (-1492553969L)
extern const struct error_table et_ovku_error_table;
extern void initialize_ovku_error_table(void);

/* For compatibility with Heimdal */
extern void initialize_ovku_error_table_r(struct et_list **list);

#define ERROR_TABLE_BASE_ovku (-1492553984L)

/* for compatibility with older versions... */
#define init_ovku_err_tbl initialize_ovku_error_table
#define ovku_err_base ERROR_TABLE_BASE_ovku