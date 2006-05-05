begin_unit
begin_ifndef
ifndef|#
directive|ifndef
name|COMMIT_H
end_ifndef
begin_define
DECL|macro|COMMIT_H
define|#
directive|define
name|COMMIT_H
end_define
begin_include
include|#
directive|include
file|"object.h"
end_include
begin_include
include|#
directive|include
file|"tree.h"
end_include
begin_struct
DECL|struct|commit_list
struct|struct
name|commit_list
block|{
DECL|member|item
name|struct
name|commit
modifier|*
name|item
decl_stmt|;
DECL|member|next
name|struct
name|commit_list
modifier|*
name|next
decl_stmt|;
block|}
struct|;
end_struct
begin_struct
DECL|struct|commit
struct|struct
name|commit
block|{
DECL|member|object
name|struct
name|object
name|object
decl_stmt|;
DECL|member|date
name|unsigned
name|long
name|date
decl_stmt|;
DECL|member|parents
name|struct
name|commit_list
modifier|*
name|parents
decl_stmt|;
DECL|member|tree
name|struct
name|tree
modifier|*
name|tree
decl_stmt|;
DECL|member|buffer
name|char
modifier|*
name|buffer
decl_stmt|;
block|}
struct|;
end_struct
begin_decl_stmt
specifier|extern
name|int
name|save_commit_buffer
decl_stmt|;
end_decl_stmt
begin_decl_stmt
specifier|extern
specifier|const
name|char
modifier|*
name|commit_type
decl_stmt|;
end_decl_stmt
begin_function_decl
name|struct
name|commit
modifier|*
name|lookup_commit
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|struct
name|commit
modifier|*
name|lookup_commit_reference
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|struct
name|commit
modifier|*
name|lookup_commit_reference_gently
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|int
name|quiet
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|parse_commit_buffer
parameter_list|(
name|struct
name|commit
modifier|*
name|item
parameter_list|,
name|void
modifier|*
name|buffer
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|parse_commit
parameter_list|(
name|struct
name|commit
modifier|*
name|item
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|struct
name|commit_list
modifier|*
name|commit_list_insert
parameter_list|(
name|struct
name|commit
modifier|*
name|item
parameter_list|,
name|struct
name|commit_list
modifier|*
modifier|*
name|list_p
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|struct
name|commit_list
modifier|*
name|insert_by_date
parameter_list|(
name|struct
name|commit
modifier|*
name|item
parameter_list|,
name|struct
name|commit_list
modifier|*
modifier|*
name|list
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|free_commit_list
parameter_list|(
name|struct
name|commit_list
modifier|*
name|list
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|sort_by_date
parameter_list|(
name|struct
name|commit_list
modifier|*
modifier|*
name|list
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/* Commit formats */
end_comment
begin_enum
DECL|enum|cmit_fmt
enum|enum
name|cmit_fmt
block|{
DECL|enumerator|CMIT_FMT_RAW
name|CMIT_FMT_RAW
block|,
DECL|enumerator|CMIT_FMT_MEDIUM
name|CMIT_FMT_MEDIUM
block|,
DECL|enumerator|CMIT_FMT_DEFAULT
name|CMIT_FMT_DEFAULT
init|=
name|CMIT_FMT_MEDIUM
block|,
DECL|enumerator|CMIT_FMT_SHORT
name|CMIT_FMT_SHORT
block|,
DECL|enumerator|CMIT_FMT_FULL
name|CMIT_FMT_FULL
block|,
DECL|enumerator|CMIT_FMT_FULLER
name|CMIT_FMT_FULLER
block|,
DECL|enumerator|CMIT_FMT_ONELINE
name|CMIT_FMT_ONELINE
block|,
DECL|enumerator|CMIT_FMT_EMAIL
name|CMIT_FMT_EMAIL
block|,
DECL|enumerator|CMIT_FMT_UNSPECIFIED
name|CMIT_FMT_UNSPECIFIED
block|, }
enum|;
end_enum
begin_function_decl
specifier|extern
name|enum
name|cmit_fmt
name|get_commit_format
parameter_list|(
specifier|const
name|char
modifier|*
name|arg
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|unsigned
name|long
name|pretty_print_commit
parameter_list|(
name|enum
name|cmit_fmt
name|fmt
parameter_list|,
specifier|const
name|struct
name|commit
modifier|*
parameter_list|,
name|unsigned
name|long
name|len
parameter_list|,
name|char
modifier|*
name|buf
parameter_list|,
name|unsigned
name|long
name|space
parameter_list|,
name|int
name|abbrev
parameter_list|,
specifier|const
name|char
modifier|*
name|subject
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/** Removes the first commit from a list sorted by date, and adds all  * of its parents.  **/
end_comment
begin_function_decl
name|struct
name|commit
modifier|*
name|pop_most_recent_commit
parameter_list|(
name|struct
name|commit_list
modifier|*
modifier|*
name|list
parameter_list|,
name|unsigned
name|int
name|mark
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|struct
name|commit
modifier|*
name|pop_commit
parameter_list|(
name|struct
name|commit_list
modifier|*
modifier|*
name|stack
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|clear_commit_marks
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|,
name|unsigned
name|int
name|mark
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|count_parents
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/*  * Performs an in-place topological sort of list supplied.  *  * Pre-conditions for sort_in_topological_order:  *   all commits in input list and all parents of those  *   commits must have object.util == NULL  *  * Pre-conditions for sort_in_topological_order_fn:  *   all commits in input list and all parents of those  *   commits must have getter(commit) == NULL  *  * Post-conditions:  *   invariant of resulting list is:  *      a reachable from b => ord(b)< ord(a)  *   in addition, when lifo == 0, commits on parallel tracks are  *   sorted in the dates order.  */
end_comment
begin_typedef
DECL|typedef|topo_sort_set_fn_t
typedef|typedef
name|void
function_decl|(
modifier|*
name|topo_sort_set_fn_t
function_decl|)
parameter_list|(
name|struct
name|commit
modifier|*
parameter_list|,
name|void
modifier|*
name|data
parameter_list|)
function_decl|;
end_typedef
begin_typedef
DECL|typedef|topo_sort_get_fn_t
typedef|typedef
name|void
modifier|*
function_decl|(
modifier|*
name|topo_sort_get_fn_t
function_decl|)
parameter_list|(
name|struct
name|commit
modifier|*
parameter_list|)
function_decl|;
end_typedef
begin_function_decl
name|void
name|topo_sort_default_setter
parameter_list|(
name|struct
name|commit
modifier|*
name|c
parameter_list|,
name|void
modifier|*
name|data
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
modifier|*
name|topo_sort_default_getter
parameter_list|(
name|struct
name|commit
modifier|*
name|c
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|sort_in_topological_order
parameter_list|(
name|struct
name|commit_list
modifier|*
modifier|*
name|list
parameter_list|,
name|int
name|lifo
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|sort_in_topological_order_fn
parameter_list|(
name|struct
name|commit_list
modifier|*
modifier|*
name|list
parameter_list|,
name|int
name|lifo
parameter_list|,
name|topo_sort_set_fn_t
name|setter
parameter_list|,
name|topo_sort_get_fn_t
name|getter
parameter_list|)
function_decl|;
end_function_decl
begin_struct
DECL|struct|commit_graft
struct|struct
name|commit_graft
block|{
DECL|member|sha1
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
DECL|member|nr_parent
name|int
name|nr_parent
decl_stmt|;
DECL|member|parent
name|unsigned
name|char
name|parent
index|[
name|FLEX_ARRAY
index|]
index|[
literal|20
index|]
decl_stmt|;
comment|/* more */
block|}
struct|;
end_struct
begin_function_decl
name|struct
name|commit_graft
modifier|*
name|read_graft_line
parameter_list|(
name|char
modifier|*
name|buf
parameter_list|,
name|int
name|len
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|register_commit_graft
parameter_list|(
name|struct
name|commit_graft
modifier|*
parameter_list|,
name|int
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|read_graft_file
parameter_list|(
specifier|const
name|char
modifier|*
name|graft_file
parameter_list|)
function_decl|;
end_function_decl
begin_endif
endif|#
directive|endif
end_endif
begin_comment
comment|/* COMMIT_H */
end_comment
end_unit
