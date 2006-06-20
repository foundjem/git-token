begin_unit
begin_ifndef
ifndef|#
directive|ifndef
name|REVISION_H
end_ifndef
begin_define
DECL|macro|REVISION_H
define|#
directive|define
name|REVISION_H
end_define
begin_define
DECL|macro|SEEN
define|#
directive|define
name|SEEN
value|(1u<<0)
end_define
begin_define
DECL|macro|UNINTERESTING
define|#
directive|define
name|UNINTERESTING
value|(1u<<1)
end_define
begin_define
DECL|macro|TREECHANGE
define|#
directive|define
name|TREECHANGE
value|(1u<<2)
end_define
begin_define
DECL|macro|SHOWN
define|#
directive|define
name|SHOWN
value|(1u<<3)
end_define
begin_define
DECL|macro|TMP_MARK
define|#
directive|define
name|TMP_MARK
value|(1u<<4)
end_define
begin_comment
DECL|macro|TMP_MARK
comment|/* for isolated cases; clean after use */
end_comment
begin_define
DECL|macro|BOUNDARY
define|#
directive|define
name|BOUNDARY
value|(1u<<5)
end_define
begin_define
DECL|macro|BOUNDARY_SHOW
define|#
directive|define
name|BOUNDARY_SHOW
value|(1u<<6)
end_define
begin_define
DECL|macro|ADDED
define|#
directive|define
name|ADDED
value|(1u<<7)
end_define
begin_comment
DECL|macro|ADDED
comment|/* Parents already parsed and added? */
end_comment
begin_struct_decl
struct_decl|struct
name|rev_info
struct_decl|;
end_struct_decl
begin_struct_decl
struct_decl|struct
name|log_info
struct_decl|;
end_struct_decl
begin_typedef
DECL|typedef|prune_fn_t
typedef|typedef
name|void
function_decl|(
name|prune_fn_t
function_decl|)
parameter_list|(
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|,
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
function_decl|;
end_typedef
begin_struct
DECL|struct|rev_info
struct|struct
name|rev_info
block|{
comment|/* Starting list */
DECL|member|commits
name|struct
name|commit_list
modifier|*
name|commits
decl_stmt|;
DECL|member|pending
name|struct
name|object_array
name|pending
decl_stmt|;
comment|/* Basic information */
DECL|member|prefix
specifier|const
name|char
modifier|*
name|prefix
decl_stmt|;
DECL|member|prune_data
name|void
modifier|*
name|prune_data
decl_stmt|;
DECL|member|prune_fn
name|prune_fn_t
modifier|*
name|prune_fn
decl_stmt|;
comment|/* Traversal flags */
DECL|member|dense
name|unsigned
name|int
name|dense
range|:
literal|1
decl_stmt|,
DECL|member|no_merges
name|no_merges
range|:
literal|1
decl_stmt|,
DECL|member|no_walk
name|no_walk
range|:
literal|1
decl_stmt|,
DECL|member|remove_empty_trees
name|remove_empty_trees
range|:
literal|1
decl_stmt|,
DECL|member|simplify_history
name|simplify_history
range|:
literal|1
decl_stmt|,
DECL|member|lifo
name|lifo
range|:
literal|1
decl_stmt|,
DECL|member|topo_order
name|topo_order
range|:
literal|1
decl_stmt|,
DECL|member|tag_objects
name|tag_objects
range|:
literal|1
decl_stmt|,
DECL|member|tree_objects
name|tree_objects
range|:
literal|1
decl_stmt|,
DECL|member|blob_objects
name|blob_objects
range|:
literal|1
decl_stmt|,
DECL|member|edge_hint
name|edge_hint
range|:
literal|1
decl_stmt|,
DECL|member|limited
name|limited
range|:
literal|1
decl_stmt|,
DECL|member|unpacked
name|unpacked
range|:
literal|1
decl_stmt|,
DECL|member|boundary
name|boundary
range|:
literal|1
decl_stmt|,
DECL|member|parents
name|parents
range|:
literal|1
decl_stmt|;
comment|/* Diff flags */
DECL|member|diff
name|unsigned
name|int
name|diff
range|:
literal|1
decl_stmt|,
DECL|member|full_diff
name|full_diff
range|:
literal|1
decl_stmt|,
DECL|member|show_root_diff
name|show_root_diff
range|:
literal|1
decl_stmt|,
DECL|member|no_commit_id
name|no_commit_id
range|:
literal|1
decl_stmt|,
DECL|member|verbose_header
name|verbose_header
range|:
literal|1
decl_stmt|,
DECL|member|ignore_merges
name|ignore_merges
range|:
literal|1
decl_stmt|,
DECL|member|combine_merges
name|combine_merges
range|:
literal|1
decl_stmt|,
DECL|member|dense_combined_merges
name|dense_combined_merges
range|:
literal|1
decl_stmt|,
DECL|member|always_show_header
name|always_show_header
range|:
literal|1
decl_stmt|;
comment|/* Format info */
DECL|member|shown_one
name|unsigned
name|int
name|shown_one
range|:
literal|1
decl_stmt|,
DECL|member|abbrev_commit
name|abbrev_commit
range|:
literal|1
decl_stmt|;
DECL|member|abbrev
name|unsigned
name|int
name|abbrev
decl_stmt|;
DECL|member|commit_format
name|enum
name|cmit_fmt
name|commit_format
decl_stmt|;
DECL|member|loginfo
name|struct
name|log_info
modifier|*
name|loginfo
decl_stmt|;
DECL|member|nr
DECL|member|total
name|int
name|nr
decl_stmt|,
name|total
decl_stmt|;
DECL|member|mime_boundary
specifier|const
name|char
modifier|*
name|mime_boundary
decl_stmt|;
DECL|member|add_signoff
specifier|const
name|char
modifier|*
name|add_signoff
decl_stmt|;
DECL|member|extra_headers
specifier|const
name|char
modifier|*
name|extra_headers
decl_stmt|;
comment|/* special limits */
DECL|member|max_count
name|int
name|max_count
decl_stmt|;
DECL|member|max_age
name|unsigned
name|long
name|max_age
decl_stmt|;
DECL|member|min_age
name|unsigned
name|long
name|min_age
decl_stmt|;
comment|/* diff info for patches and for paths limiting */
DECL|member|diffopt
name|struct
name|diff_options
name|diffopt
decl_stmt|;
DECL|member|pruning
name|struct
name|diff_options
name|pruning
decl_stmt|;
DECL|member|topo_setter
name|topo_sort_set_fn_t
name|topo_setter
decl_stmt|;
DECL|member|topo_getter
name|topo_sort_get_fn_t
name|topo_getter
decl_stmt|;
block|}
struct|;
end_struct
begin_define
DECL|macro|REV_TREE_SAME
define|#
directive|define
name|REV_TREE_SAME
value|0
end_define
begin_define
DECL|macro|REV_TREE_NEW
define|#
directive|define
name|REV_TREE_NEW
value|1
end_define
begin_define
DECL|macro|REV_TREE_DIFFERENT
define|#
directive|define
name|REV_TREE_DIFFERENT
value|2
end_define
begin_comment
comment|/* revision.c */
end_comment
begin_function_decl
specifier|extern
name|int
name|rev_same_tree_as_empty
parameter_list|(
name|struct
name|rev_info
modifier|*
parameter_list|,
name|struct
name|tree
modifier|*
name|t1
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|rev_compare_tree
parameter_list|(
name|struct
name|rev_info
modifier|*
parameter_list|,
name|struct
name|tree
modifier|*
name|t1
parameter_list|,
name|struct
name|tree
modifier|*
name|t2
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|init_revisions
parameter_list|(
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|setup_revisions
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
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|,
specifier|const
name|char
modifier|*
name|def
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|prepare_revision_walk
parameter_list|(
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|struct
name|commit
modifier|*
name|get_revision
parameter_list|(
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|mark_parents_uninteresting
parameter_list|(
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
name|mark_tree_uninteresting
parameter_list|(
name|struct
name|tree
modifier|*
name|tree
parameter_list|)
function_decl|;
end_function_decl
begin_struct
DECL|struct|name_path
struct|struct
name|name_path
block|{
DECL|member|up
name|struct
name|name_path
modifier|*
name|up
decl_stmt|;
DECL|member|elem_len
name|int
name|elem_len
decl_stmt|;
DECL|member|elem
specifier|const
name|char
modifier|*
name|elem
decl_stmt|;
block|}
struct|;
end_struct
begin_function_decl
specifier|extern
name|void
name|add_object
parameter_list|(
name|struct
name|object
modifier|*
name|obj
parameter_list|,
name|struct
name|object_array
modifier|*
name|p
parameter_list|,
name|struct
name|name_path
modifier|*
name|path
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|add_pending_object
parameter_list|(
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|,
name|struct
name|object
modifier|*
name|obj
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|)
function_decl|;
end_function_decl
begin_endif
endif|#
directive|endif
end_endif
end_unit
