begin_unit
begin_ifndef
ifndef|#
directive|ifndef
name|MERGE_RECURSIVE_H
end_ifndef
begin_define
DECL|macro|MERGE_RECURSIVE_H
define|#
directive|define
name|MERGE_RECURSIVE_H
end_define
begin_include
include|#
directive|include
file|"string-list.h"
end_include
begin_struct
DECL|struct|merge_options
struct|struct
name|merge_options
block|{
DECL|member|branch1
specifier|const
name|char
modifier|*
name|branch1
decl_stmt|;
DECL|member|branch2
specifier|const
name|char
modifier|*
name|branch2
decl_stmt|;
enum|enum
block|{
DECL|enumerator|MERGE_RECURSIVE_NORMAL
name|MERGE_RECURSIVE_NORMAL
init|=
literal|0
block|,
DECL|enumerator|MERGE_RECURSIVE_OURS
name|MERGE_RECURSIVE_OURS
block|,
DECL|enumerator|MERGE_RECURSIVE_THEIRS
name|MERGE_RECURSIVE_THEIRS
block|, 	}
DECL|member|recursive_variant
name|recursive_variant
enum|;
DECL|member|subtree_shift
specifier|const
name|char
modifier|*
name|subtree_shift
decl_stmt|;
DECL|member|buffer_output
name|unsigned
name|buffer_output
range|:
literal|1
decl_stmt|;
DECL|member|verbosity
name|int
name|verbosity
decl_stmt|;
DECL|member|diff_rename_limit
name|int
name|diff_rename_limit
decl_stmt|;
DECL|member|merge_rename_limit
name|int
name|merge_rename_limit
decl_stmt|;
DECL|member|call_depth
name|int
name|call_depth
decl_stmt|;
DECL|member|obuf
name|struct
name|strbuf
name|obuf
decl_stmt|;
DECL|member|current_file_set
name|struct
name|string_list
name|current_file_set
decl_stmt|;
DECL|member|current_directory_set
name|struct
name|string_list
name|current_directory_set
decl_stmt|;
block|}
struct|;
end_struct
begin_comment
comment|/* Return a list of user-friendly error messages to be used by merge */
end_comment
begin_function_decl
name|struct
name|unpack_trees_error_msgs
name|get_porcelain_error_msgs
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/* merge_trees() but with recursive ancestor consolidation */
end_comment
begin_function_decl
name|int
name|merge_recursive
parameter_list|(
name|struct
name|merge_options
modifier|*
name|o
parameter_list|,
name|struct
name|commit
modifier|*
name|h1
parameter_list|,
name|struct
name|commit
modifier|*
name|h2
parameter_list|,
name|struct
name|commit_list
modifier|*
name|ancestors
parameter_list|,
name|struct
name|commit
modifier|*
modifier|*
name|result
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/* rename-detecting three-way merge, no recursion */
end_comment
begin_function_decl
name|int
name|merge_trees
parameter_list|(
name|struct
name|merge_options
modifier|*
name|o
parameter_list|,
name|struct
name|tree
modifier|*
name|head
parameter_list|,
name|struct
name|tree
modifier|*
name|merge
parameter_list|,
name|struct
name|tree
modifier|*
name|common
parameter_list|,
name|struct
name|tree
modifier|*
modifier|*
name|result
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/*  * "git-merge-recursive" can be fed trees; wrap them into  * virtual commits and call merge_recursive() proper.  */
end_comment
begin_function_decl
name|int
name|merge_recursive_generic
parameter_list|(
name|struct
name|merge_options
modifier|*
name|o
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|head
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|merge
parameter_list|,
name|int
name|num_ca
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
modifier|*
name|ca
parameter_list|,
name|struct
name|commit
modifier|*
modifier|*
name|result
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|init_merge_options
parameter_list|(
name|struct
name|merge_options
modifier|*
name|o
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|struct
name|tree
modifier|*
name|write_tree_from_memory
parameter_list|(
name|struct
name|merge_options
modifier|*
name|o
parameter_list|)
function_decl|;
end_function_decl
begin_endif
endif|#
directive|endif
end_endif
end_unit
