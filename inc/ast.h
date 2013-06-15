struct ast_node {
	char *token; // ?
	/** null-terminated array of child nodes */
	struct ast_node **children;
};

extern void ast_to_string_tree(struct ast* ast);
