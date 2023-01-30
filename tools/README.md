# Generate HLSL Token Types

There are a lot of HLSL keywords. To assist in authoring the _token_types.h_ and
_token_types.cpp_ source files, the HLSL keywords are listed in _keywords.txt_, _tokens.txt_,
and _vector_matrix_types.txt_.

Run **gen_token_type.py** in this folder to generate _tokens_types.h_ and _token_types.cpp_.

This only needs to be done if the list of keywords changes.
