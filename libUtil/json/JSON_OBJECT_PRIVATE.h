#ifndef _json_object_private_h_k
#define _json_object_private_h_k

#ifdef __cplusplus
extern "C" {
#endif
namespace nsUtil
{
typedef void (json_object_delete_fn)(struct json_object *o);
typedef int (json_object_to_json_string_fn)(struct json_object *o,
                   struct printbuf *pb);

struct json_object
{
  enum json_type o_type;
  json_object_delete_fn *_delete;
  json_object_to_json_string_fn *_to_json_string;
  int _ref_count;
  struct printbuf *_pb;
  union data {
    boolean c_boolean;
    double c_double;
    long long c_int;
    struct lh_table *c_object;
    struct array_list *c_array;
    char *c_string;
  } o;
};

/* CAW: added for ANSI C iteration correctness */
struct json_object_iter
{
   char *key;
   struct json_object *val;
   struct lh_entry *entry;
};
}
#ifdef __cplusplus
}
#endif

#endif

