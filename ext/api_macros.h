
#define MCHECKERROR(STAT,MSG)       \
    if ( MS::kSuccess != STAT ) {   \
        cerr << MSG << endl;        \
            return MS::kFailure;    \
    }

#define MCHECKERRORNORET(STAT,MSG)  \
    if ( MS::kSuccess != STAT ) {   \
        cerr << MSG << endl;        \
    }

//////////////////////////////////////////////////////////////////////
//
// Attribute creation
//
//       MAKE_TYPED_ATTR   - creates and adds a typed attribute
//       MAKE_NUMERIC_ATTR - creates and adds a numeric attribute
//       ADD_ATTRIBUTE     - adds the given attribute
//       ATTRIBUTE_AFFECTS - calls attributeAffects
//
//////////////////////////////////////////////////////////////////////

#define MAKE_TYPED_ATTR( NAME, LONGNAME, SHORTNAME, TYPE, DEFAULT )         \
										                                    \
	MStatus NAME##_stat;                                                    \
	MFnTypedAttribute NAME##_fn;                                            \
	NAME = NAME##_fn.create( LONGNAME, SHORTNAME, TYPE, DEFAULT );          \
	NAME##_fn.setHidden( true );											\
	NAME##_stat = addAttribute( NAME );                                     \
	MCHECKERROR(NAME##_stat, "addAttribute error");

#define MAKE_NUMERIC_ATTR( NAME, LONGNAME, SHORTNAME, TYPE, DEFAULT,        \
							ARRAY, BUILDER, KEYABLE )                       \
										                                    \
	MStatus NAME##_stat;                                                    \
	MFnNumericAttribute NAME##_fn;                                          \
	NAME = NAME##_fn.create( LONGNAME, SHORTNAME, TYPE, DEFAULT );          \
	MCHECKERROR(NAME##_stat, "numeric attr create error");		            \
    NAME##_fn.setArray( ARRAY );                                            \
    NAME##_fn.setUsesArrayDataBuilder( BUILDER );                           \
	NAME##_fn.setHidden( ARRAY );											\
	NAME##_fn.setKeyable( KEYABLE );										\
	NAME##_stat = addAttribute( NAME );                                     \
	MCHECKERROR(NAME##_stat, "addAttribute error");

#define ADD_ATTRIBUTE( ATTR )                                               \
	MStatus ATTR##_stat;                                                    \
	ATTR##_stat = addAttribute( ATTR );                                     \
    MCHECKERROR( ATTR##_stat, "addAttribute: ATTR" )

#define ATTRIBUTE_AFFECTS( IN, OUT )                                        \
	MStatus IN##OUT##_stat;                                                 \
	IN##OUT##_stat = attributeAffects( IN, OUT );                           \
	MCHECKERROR(IN##OUT##_stat,"attributeAffects:" #IN "->" #OUT);

