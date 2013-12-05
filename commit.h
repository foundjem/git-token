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
begin_include
include|#
directive|include
file|"strbuf.h"
end_include
begin_include
include|#
directive|include
file|"decorate.h"
end_include
begin_include
include|#
directive|include
file|"gpg-interface.h"
end_include
begin_include
include|#
directive|include
file|"string-list.h"
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
DECL|member|util
name|void
modifier|*
name|util
decl_stmt|;
DECL|member|index
name|unsigned
name|int
name|index
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
begin_comment
comment|/* While we can decorate any object with a name, it's only used for commits.. */
end_comment
begin_decl_stmt
specifier|extern
name|struct
name|decoration
name|name_decoration
decl_stmt|;
end_decl_stmt
begin_struct
DECL|struct|name_decoration
struct|struct
name|name_decoration
block|{
DECL|member|next
name|struct
name|name_decoration
modifier|*
name|next
decl_stmt|;
DECL|member|type
name|int
name|type
decl_stmt|;
DECL|member|name
name|char
name|name
index|[
literal|1
index|]
decl_stmt|;
block|}
struct|;
end_struct
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
name|struct
name|commit
modifier|*
name|lookup_commit_reference_by_name
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/*  * Look up object named by "sha1", dereference tag as necessary,  * get a commit and return it. If "sha1" does not dereference to  * a commit, use ref_name to report an error and die.  */
end_comment
begin_function_decl
name|struct
name|commit
modifier|*
name|lookup_commit_or_die
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|ref_name
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
specifier|const
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
begin_comment
comment|/* Find beginning and length of commit subject. */
end_comment
begin_function_decl
name|int
name|find_commit_subject
parameter_list|(
specifier|const
name|char
modifier|*
name|commit_buffer
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|subject
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
name|list
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|struct
name|commit_list
modifier|*
modifier|*
name|commit_list_append
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|,
name|struct
name|commit_list
modifier|*
modifier|*
name|next
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|unsigned
name|commit_list_count
parameter_list|(
specifier|const
name|struct
name|commit_list
modifier|*
name|l
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|struct
name|commit_list
modifier|*
name|commit_list_insert_by_date
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
name|commit_list_sort_by_date
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
comment|/* Shallow copy of the input list */
end_comment
begin_function_decl
name|struct
name|commit_list
modifier|*
name|copy_commit_list
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
name|free_commit_list
parameter_list|(
name|struct
name|commit_list
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
DECL|enumerator|CMIT_FMT_USERFORMAT
name|CMIT_FMT_USERFORMAT
block|,
DECL|enumerator|CMIT_FMT_UNSPECIFIED
name|CMIT_FMT_UNSPECIFIED
block|}
enum|;
end_enum
begin_struct
DECL|struct|pretty_print_context
struct|struct
name|pretty_print_context
block|{
comment|/* 	 * Callers should tweak these to change the behavior of pp_* functions. 	 */
DECL|member|fmt
name|enum
name|cmit_fmt
name|fmt
decl_stmt|;
DECL|member|abbrev
name|int
name|abbrev
decl_stmt|;
DECL|member|subject
specifier|const
name|char
modifier|*
name|subject
decl_stmt|;
DECL|member|after_subject
specifier|const
name|char
modifier|*
name|after_subject
decl_stmt|;
DECL|member|preserve_subject
name|int
name|preserve_subject
decl_stmt|;
DECL|member|date_mode
name|enum
name|date_mode
name|date_mode
decl_stmt|;
DECL|member|date_mode_explicit
name|unsigned
name|date_mode_explicit
range|:
literal|1
decl_stmt|;
DECL|member|need_8bit_cte
name|int
name|need_8bit_cte
decl_stmt|;
DECL|member|notes_message
name|char
modifier|*
name|notes_message
decl_stmt|;
DECL|member|reflog_info
name|struct
name|reflog_walk_info
modifier|*
name|reflog_info
decl_stmt|;
DECL|member|output_encoding
specifier|const
name|char
modifier|*
name|output_encoding
decl_stmt|;
DECL|member|mailmap
name|struct
name|string_list
modifier|*
name|mailmap
decl_stmt|;
DECL|member|color
name|int
name|color
decl_stmt|;
DECL|member|from_ident
name|struct
name|ident_split
modifier|*
name|from_ident
decl_stmt|;
comment|/* 	 * Fields below here are manipulated internally by pp_* functions and 	 * should not be counted on by callers. 	 */
DECL|member|in_body_headers
name|struct
name|string_list
name|in_body_headers
decl_stmt|;
block|}
struct|;
end_struct
begin_struct
DECL|struct|userformat_want
struct|struct
name|userformat_want
block|{
DECL|member|notes
name|unsigned
name|notes
range|:
literal|1
decl_stmt|;
block|}
struct|;
end_struct
begin_function_decl
specifier|extern
name|int
name|has_non_ascii
parameter_list|(
specifier|const
name|char
modifier|*
name|text
parameter_list|)
function_decl|;
end_function_decl
begin_struct_decl
struct_decl|struct
name|rev_info
struct_decl|;
end_struct_decl
begin_comment
comment|/* in revision.h, it circularly uses enum cmit_fmt */
end_comment
begin_function_decl
specifier|extern
name|char
modifier|*
name|logmsg_reencode
parameter_list|(
specifier|const
name|struct
name|commit
modifier|*
name|commit
parameter_list|,
name|char
modifier|*
modifier|*
name|commit_encoding
parameter_list|,
specifier|const
name|char
modifier|*
name|output_encoding
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|logmsg_free
parameter_list|(
name|char
modifier|*
name|msg
parameter_list|,
specifier|const
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|get_commit_format
parameter_list|(
specifier|const
name|char
modifier|*
name|arg
parameter_list|,
name|struct
name|rev_info
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
specifier|const
name|char
modifier|*
name|format_subject
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|char
modifier|*
name|msg
parameter_list|,
specifier|const
name|char
modifier|*
name|line_separator
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|userformat_find_requirements
parameter_list|(
specifier|const
name|char
modifier|*
name|fmt
parameter_list|,
name|struct
name|userformat_want
modifier|*
name|w
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|format_commit_message
parameter_list|(
specifier|const
name|struct
name|commit
modifier|*
name|commit
parameter_list|,
specifier|const
name|char
modifier|*
name|format
parameter_list|,
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|struct
name|pretty_print_context
modifier|*
name|context
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|pretty_print_commit
parameter_list|(
name|struct
name|pretty_print_context
modifier|*
name|pp
parameter_list|,
specifier|const
name|struct
name|commit
modifier|*
name|commit
parameter_list|,
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|pp_commit_easy
parameter_list|(
name|enum
name|cmit_fmt
name|fmt
parameter_list|,
specifier|const
name|struct
name|commit
modifier|*
name|commit
parameter_list|,
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|pp_user_info
parameter_list|(
name|struct
name|pretty_print_context
modifier|*
name|pp
parameter_list|,
specifier|const
name|char
modifier|*
name|what
parameter_list|,
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|char
modifier|*
name|line
parameter_list|,
specifier|const
name|char
modifier|*
name|encoding
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|pp_title_line
parameter_list|(
name|struct
name|pretty_print_context
modifier|*
name|pp
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|msg_p
parameter_list|,
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|char
modifier|*
name|encoding
parameter_list|,
name|int
name|need_8bit_cte
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|pp_remainder
parameter_list|(
name|struct
name|pretty_print_context
modifier|*
name|pp
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|msg_p
parameter_list|,
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
name|int
name|indent
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
name|void
name|clear_commit_marks_many
parameter_list|(
name|int
name|nr
parameter_list|,
name|struct
name|commit
modifier|*
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
name|void
name|clear_commit_marks_for_object_array
parameter_list|(
name|struct
name|object_array
modifier|*
name|a
parameter_list|,
name|unsigned
name|mark
parameter_list|)
function_decl|;
end_function_decl
begin_enum
DECL|enum|rev_sort_order
enum|enum
name|rev_sort_order
block|{
DECL|enumerator|REV_SORT_IN_GRAPH_ORDER
name|REV_SORT_IN_GRAPH_ORDER
init|=
literal|0
block|,
DECL|enumerator|REV_SORT_BY_COMMIT_DATE
name|REV_SORT_BY_COMMIT_DATE
block|,
DECL|enumerator|REV_SORT_BY_AUTHOR_DATE
name|REV_SORT_BY_AUTHOR_DATE
block|}
enum|;
end_enum
begin_comment
comment|/*  * Performs an in-place topological sort of list supplied.  *  *   invariant of resulting list is:  *      a reachable from b => ord(b)< ord(a)  *   sort_order further specifies:  *   REV_SORT_IN_GRAPH_ORDER: try to show a commit on a single-parent  *                            chain together.  *   REV_SORT_BY_COMMIT_DATE: show eligible commits in committer-date order.  */
end_comment
begin_function_decl
name|void
name|sort_in_topological_order
parameter_list|(
name|struct
name|commit_list
modifier|*
modifier|*
parameter_list|,
name|enum
name|rev_sort_order
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
comment|/*< 0 if shallow commit */
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
begin_typedef
DECL|typedef|each_commit_graft_fn
typedef|typedef
name|int
function_decl|(
modifier|*
name|each_commit_graft_fn
function_decl|)
parameter_list|(
specifier|const
name|struct
name|commit_graft
modifier|*
parameter_list|,
name|void
modifier|*
parameter_list|)
function_decl|;
end_typedef
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
name|struct
name|commit_graft
modifier|*
name|lookup_commit_graft
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
specifier|extern
name|struct
name|commit_list
modifier|*
name|get_merge_bases
parameter_list|(
name|struct
name|commit
modifier|*
name|rev1
parameter_list|,
name|struct
name|commit
modifier|*
name|rev2
parameter_list|,
name|int
name|cleanup
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|struct
name|commit_list
modifier|*
name|get_merge_bases_many
parameter_list|(
name|struct
name|commit
modifier|*
name|one
parameter_list|,
name|int
name|n
parameter_list|,
name|struct
name|commit
modifier|*
modifier|*
name|twos
parameter_list|,
name|int
name|cleanup
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|struct
name|commit_list
modifier|*
name|get_octopus_merge_bases
parameter_list|(
name|struct
name|commit_list
modifier|*
name|in
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/* largest positive number a signed 32-bit integer can contain */
end_comment
begin_define
DECL|macro|INFINITE_DEPTH
define|#
directive|define
name|INFINITE_DEPTH
value|0x7fffffff
end_define
begin_struct_decl
struct_decl|struct
name|sha1_array
struct_decl|;
end_struct_decl
begin_struct_decl
struct_decl|struct
name|ref
struct_decl|;
end_struct_decl
begin_function_decl
specifier|extern
name|int
name|register_shallow
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
specifier|extern
name|int
name|unregister_shallow
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
specifier|extern
name|int
name|for_each_commit_graft
parameter_list|(
name|each_commit_graft_fn
parameter_list|,
name|void
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|is_repository_shallow
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|struct
name|commit_list
modifier|*
name|get_shallow_commits
parameter_list|(
name|struct
name|object_array
modifier|*
name|heads
parameter_list|,
name|int
name|depth
parameter_list|,
name|int
name|shallow_flag
parameter_list|,
name|int
name|not_shallow_flag
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|check_shallow_file_for_update
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|set_alternate_shallow_file
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|int
name|override
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|write_shallow_commits
parameter_list|(
name|struct
name|strbuf
modifier|*
name|out
parameter_list|,
name|int
name|use_pack_protocol
parameter_list|,
specifier|const
name|struct
name|sha1_array
modifier|*
name|extra
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|setup_alternate_shallow
parameter_list|(
name|struct
name|lock_file
modifier|*
name|shallow_lock
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|alternate_shallow_file
parameter_list|,
specifier|const
name|struct
name|sha1_array
modifier|*
name|extra
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|char
modifier|*
name|setup_temporary_shallow
parameter_list|(
specifier|const
name|struct
name|sha1_array
modifier|*
name|extra
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|advertise_shallow_grafts
parameter_list|(
name|int
parameter_list|)
function_decl|;
end_function_decl
begin_struct
DECL|struct|shallow_info
struct|struct
name|shallow_info
block|{
DECL|member|shallow
name|struct
name|sha1_array
modifier|*
name|shallow
decl_stmt|;
DECL|member|ours
DECL|member|nr_ours
name|int
modifier|*
name|ours
decl_stmt|,
name|nr_ours
decl_stmt|;
DECL|member|theirs
DECL|member|nr_theirs
name|int
modifier|*
name|theirs
decl_stmt|,
name|nr_theirs
decl_stmt|;
DECL|member|ref
name|struct
name|sha1_array
modifier|*
name|ref
decl_stmt|;
block|}
struct|;
end_struct
begin_function_decl
specifier|extern
name|void
name|prepare_shallow_info
parameter_list|(
name|struct
name|shallow_info
modifier|*
parameter_list|,
name|struct
name|sha1_array
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|clear_shallow_info
parameter_list|(
name|struct
name|shallow_info
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|remove_nonexistent_theirs_shallow
parameter_list|(
name|struct
name|shallow_info
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|remove_nonexistent_ours_in_pack
parameter_list|(
name|struct
name|shallow_info
modifier|*
parameter_list|,
name|struct
name|packed_git
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|assign_shallow_commits_to_refs
parameter_list|(
name|struct
name|shallow_info
modifier|*
name|info
parameter_list|,
name|uint32_t
modifier|*
modifier|*
name|used
parameter_list|,
name|int
modifier|*
name|ref_status
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|is_descendant_of
parameter_list|(
name|struct
name|commit
modifier|*
parameter_list|,
name|struct
name|commit_list
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|in_merge_bases
parameter_list|(
name|struct
name|commit
modifier|*
parameter_list|,
name|struct
name|commit
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|in_merge_bases_many
parameter_list|(
name|struct
name|commit
modifier|*
parameter_list|,
name|int
parameter_list|,
name|struct
name|commit
modifier|*
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|interactive_add
parameter_list|(
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|,
name|int
name|patch
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|run_add_interactive
parameter_list|(
specifier|const
name|char
modifier|*
name|revision
parameter_list|,
specifier|const
name|char
modifier|*
name|patch_mode
parameter_list|,
specifier|const
name|struct
name|pathspec
modifier|*
name|pathspec
parameter_list|)
function_decl|;
end_function_decl
begin_function
DECL|function|single_parent
specifier|static
specifier|inline
name|int
name|single_parent
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
block|{
return|return
name|commit
operator|->
name|parents
operator|&&
operator|!
name|commit
operator|->
name|parents
operator|->
name|next
return|;
block|}
end_function
begin_function_decl
name|struct
name|commit_list
modifier|*
name|reduce_heads
parameter_list|(
name|struct
name|commit_list
modifier|*
name|heads
parameter_list|)
function_decl|;
end_function_decl
begin_struct
DECL|struct|commit_extra_header
struct|struct
name|commit_extra_header
block|{
DECL|member|next
name|struct
name|commit_extra_header
modifier|*
name|next
decl_stmt|;
DECL|member|key
name|char
modifier|*
name|key
decl_stmt|;
DECL|member|value
name|char
modifier|*
name|value
decl_stmt|;
DECL|member|len
name|size_t
name|len
decl_stmt|;
block|}
struct|;
end_struct
begin_function_decl
specifier|extern
name|void
name|append_merge_tag_headers
parameter_list|(
name|struct
name|commit_list
modifier|*
name|parents
parameter_list|,
name|struct
name|commit_extra_header
modifier|*
modifier|*
modifier|*
name|tail
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|commit_tree
parameter_list|(
specifier|const
name|struct
name|strbuf
modifier|*
name|msg
parameter_list|,
name|unsigned
name|char
modifier|*
name|tree
parameter_list|,
name|struct
name|commit_list
modifier|*
name|parents
parameter_list|,
name|unsigned
name|char
modifier|*
name|ret
parameter_list|,
specifier|const
name|char
modifier|*
name|author
parameter_list|,
specifier|const
name|char
modifier|*
name|sign_commit
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|commit_tree_extended
parameter_list|(
specifier|const
name|struct
name|strbuf
modifier|*
name|msg
parameter_list|,
name|unsigned
name|char
modifier|*
name|tree
parameter_list|,
name|struct
name|commit_list
modifier|*
name|parents
parameter_list|,
name|unsigned
name|char
modifier|*
name|ret
parameter_list|,
specifier|const
name|char
modifier|*
name|author
parameter_list|,
specifier|const
name|char
modifier|*
name|sign_commit
parameter_list|,
name|struct
name|commit_extra_header
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|struct
name|commit_extra_header
modifier|*
name|read_commit_extra_headers
parameter_list|(
name|struct
name|commit
modifier|*
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|free_commit_extra_headers
parameter_list|(
name|struct
name|commit_extra_header
modifier|*
name|extra
parameter_list|)
function_decl|;
end_function_decl
begin_struct
DECL|struct|merge_remote_desc
struct|struct
name|merge_remote_desc
block|{
DECL|member|obj
name|struct
name|object
modifier|*
name|obj
decl_stmt|;
comment|/* the named object, could be a tag */
DECL|member|name
specifier|const
name|char
modifier|*
name|name
decl_stmt|;
block|}
struct|;
end_struct
begin_define
DECL|macro|merge_remote_util
define|#
directive|define
name|merge_remote_util
parameter_list|(
name|commit
parameter_list|)
value|((struct merge_remote_desc *)((commit)->util))
end_define
begin_comment
comment|/*  * Given "name" from the command line to merge, find the commit object  * and return it, while storing merge_remote_desc in its ->util field,  * to allow callers to tell if we are told to merge a tag.  */
end_comment
begin_function_decl
name|struct
name|commit
modifier|*
name|get_merge_parent
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|parse_signed_commit
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|struct
name|strbuf
modifier|*
name|message
parameter_list|,
name|struct
name|strbuf
modifier|*
name|signature
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|print_commit_list
parameter_list|(
name|struct
name|commit_list
modifier|*
name|list
parameter_list|,
specifier|const
name|char
modifier|*
name|format_cur
parameter_list|,
specifier|const
name|char
modifier|*
name|format_last
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/*  * Check the signature of the given commit. The result of the check is stored  * in sig->check_result, 'G' for a good signature, 'U' for a good signature  * from an untrusted signer, 'B' for a bad signature and 'N' for no signature  * at all.  This may allocate memory for sig->gpg_output, sig->gpg_status,  * sig->signer and sig->key.  */
end_comment
begin_function_decl
specifier|extern
name|void
name|check_commit_signature
parameter_list|(
specifier|const
name|struct
name|commit
modifier|*
name|commit
parameter_list|,
name|struct
name|signature_check
modifier|*
name|sigc
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|compare_commits_by_commit_date
parameter_list|(
specifier|const
name|void
modifier|*
name|a_
parameter_list|,
specifier|const
name|void
modifier|*
name|b_
parameter_list|,
name|void
modifier|*
name|unused
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
