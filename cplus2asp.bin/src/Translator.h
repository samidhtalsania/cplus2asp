#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <list>
#include <fstream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include "types.h"

#include "parser_types.h"
#include "ElementCounter.h"
#include "SymbolTable.h"
#include "Context.h"
#include "Sort.h"
#include "Element.h"

class Constant;
class Object;
class Variable;
class Query;
class ParseElement;
class NumberRange;
struct YYLTYPE;

/**
 * Class that provides data storage and translation capabilities.
 * Encapsulates a symbol table and data structures for the declared symbols,
 * and provides methods to manipulate and translate the stored data.
 */
class Translator
{
private:
	SymbolTable symbols; 							///< Data structure to track what symbols have been declared.
	std::list<Constant*> constants; 				///< Holds declared constants & associated data.
	std::list<Object*> objects; 					///< Holds declared objects (to populate Sort objects' domains) & associated data.
	std::list<Sort*> sorts; 						///< Holds declared sorts & associated data.
	std::list<Variable*> variables; 				///< Holds declared variables & associated data.
	std::list<Query*> queries; 						///< Holds declared queries & associated data.
	StmtList mFooterStmts;							///< Contains a list of statements that should be added at the end of the translation file. These must be added manually by calling appendFooter() after translating.
	std::ostream* ostOutPtr; 						///< Used to output translation results.
	std::ostream* ostErrPtr; 						///< Used to output error messages during translation.
	std::ostream* ostNullPtr; 						///< Used internally to suppress output or flag when an output stream has not been set.
	std::fstream fNull; 							///< Used internally to open a null file handle (for use with ostNull).
	std::ostringstream ossErr; 						///< String stream used to generate & hold error messages before output.
	bool blnStaticTrans; 							///< True if we should output a static translation instead of a dynamic one.

	bool blnFoundAbnormalities; 	///< True if the translator has encountered static or dynamic abnormality constants.
	bool blnFoundAdditive;			///< True if the translator has encountered additive constants.
	bool blnEncounteredShowStmt;	///< True if the translator has encountered at least one show statement.


	IPart mCurrentPart; ///< The current incremental part that the translation is using.


	std::string staticTimeStamp; 		// Represents the time stamp used for static portions of the causal laws.
	std::string dynamicTimeStamp;		// Represents the time stamp used for dynamic portions of the causal laws.

	enum RuleType { RULE_STATIC, RULE_ACTIONDYNAMIC, RULE_FLUENTDYNAMIC, RULE_RIGID };	///< An enum describing the types of rules we can have.

public:
	int translatorNumErrors; ///< Tracks how many errors the translator module encounters while translating things.
	int translatorNumWarnings;	///< Tracks how many warnings the translator module encounters during operation.
	Query* tempQuery; ///< Helps the parser process and prepare queries for translation.
	
	/**
	 * Default constructor. Initializes attributes to blank/empty.
	 * Also sets up default internal sorts, objects, etc.
	 */
	Translator();
	
	/* Static methods for general translation/sanitizing of element names. */
	
	/**
	 * Transforms a C+ constant name to be compatible with ASP naming/syntax conventions.
	 * Uses a consistent sanitization scheme, so calling it twice on the same
	 * input will produce the same output both times.
	 * @param originalName - The original base name of the constant.
	 * @return A sanitized name for the constant, or "" (blank) on an error.
	 */
	static std::string sanitizeConstantName(std::string const& originalName);
	/**
	 * Transforms a C+ object name to be compatible with ASP naming/syntax conventions.
	 * Uses a consistent sanitization scheme, so calling it twice on the same
	 * input will produce the same output both times.
	 * @param originalName - The original base name of the object.
	 * @return A sanitized name for the object, or "" (blank) on an error.
	 */
	static std::string sanitizeObjectName(std::string const& originalName);
	/**
	 * Transforms a C+ sort name to be compatible with ASP naming/syntax conventions.
	 * Uses a consistent sanitization scheme, so calling it twice on the same
	 * input will produce the same output both times.
	 * @param originalName - The original base name of the sort.
	 * @return A sanitized name for the sort, or "" (blank) on an error.
	 */
	static std::string sanitizeSortName(std::string const& originalName);
	/**
	 * Transforms a C+ variable name to be compatible with ASP naming/syntax conventions.
	 * Uses a consistent sanitization scheme, so calling it twice on the same
	 * input will produce the same output both times.
	 * @param originalName - The original base name of the variable.
	 * @return A sanitized name for the variable, or "" (blank) on an error.
	 */
	static std::string sanitizeVariableName(std::string const& originalName);
	
	/**
	 * Transforms an arbitrary string such that the result will be compatible
	 * with ASP object/predicate naming and syntax conventions.
	 * Uses a consistent sanitization scheme, so calling it twice on the same
	 * input will produce the same output both times.
	 * @param originalString - The raw string to sanitize.
	 * @return A sanitized version of the string, or "" (blank) on an error.
	 */
	static std::string sanitizeString(std::string const& originalString);
	
	/**
	 * Static version of translateVariableDecl that returns the result as
	 * a string instead of sending the result to an output stream.
	 * Translates a Variable element into an ASP-compatible variable declaration.
	 * If the variable's sort reference is NULL, will return a blank string.
	 * @param transVar - The Variable element to translate.
	 * @return A string containing the ASP-equivalent variable declaration,
	 * or a blank string ("") on an error.
	 */
	static std::string translateVariableDeclToString(Variable const* transVar);
	
	/**
	 * Creates a standardized sort name representing a range of numbers.
	 * Needed when a number range is given as the binding sort of certain
	 * declarations.
	 * Uses a consistent naming convention, so calling it twice on the same
	 * range produces identical results.
	 * @param min - The lower bound of the range.
	 * @param max - The upper bound of the range.
	 * @return A representation of the number range suitable for use as a
	 * sort name, or "" (blank) on an error.
	 */
	static std::string numRangeToSortName(std::string const& min, std::string const& max);
	
	/**
	 * Transforms a sort name into an acceptable variable name containing
	 * as much of the original sort name as possible.
	 * Uses a consistent naming convention, so calling it twice on the same
	 * range produces identical results.
	 * @param sortName - The sort name to transform.
	 * @param sanitizeFirst - If true, will pass the sort name through
	 * sanitizeSortName before transforming it.
	 * @return An acceptable variable representation of the sort name, or ""
	 * (blank) on an error.
	 */
	static std::string sortNameToVariable(std::string const& sortName, bool sanitizeFirst=false);
	
	/**
	 * Merges two sub-formulas together into one formula using an AND operation.
	 * @param first - The first sub-formula of the pair.
	 * @param second - The second sub-formula of the pair.
	 * @return A dynamically-declared ParseElement pointer represnting the merging of the two sub-formulas.
	 */
	static ParseElement* mergeSubFormulas(ParseElement* first, ParseElement* second);
	
	/* Accessors that (attempt to) retrieve an instance of a declared element. */
	
	/**
	 * Determines if the translator has encountered any abnormality constants.
	 * @return True if one or more static or dynamic abnormalities have been declared.
	 */
	inline bool hasFoundAbnormalities() const { return blnFoundAbnormalities; }

	/**
	 * Determines if the translator has encountered any additive constants.
	 * @return True if one or more additive constants have been declared.
	 */
	inline bool hasFoundAdditiveConstants() const { return blnFoundAdditive; }

	/**
	 * Searches for a constant matching the given name and parameters in the
	 * translator's data structures.
	 * @param symName - The base name of the constant to find.
	 * @param symParams - A vector of parameter names for the constant. NULL to indicate no parameters.
	 * @return A pointer to the first matching Constant object, or NULL
	 * if a match isn't found.
	 */
	Constant* getConstant(std::string const& symName, NameList const& symParams);


	/**
	 * Searches for a constant matching the given name and parameters in the
	 * translator's data structures.
	 * @param symName - The base name of the constant to find.
	 * @param symParams - A list of all the sort's which correspond to the constant's parameters.
	 * @return A pointer to the first matching Constant object, or NULL
	 * if a match isn't found.
	 */
	Constant* getConstant(std::string const& symName, SortList const* symParams);


	/**
	 * Searches for an object matching the given name and parameters in the
	 * translator's data structures.
	 * @param symName - The base name of the object to find.
	 * @param symParams - A vector of parameter names for the object. NULL to indicate no parameters.
	 * @return A pointer to the first matching Object instance, or NULL
	 * if a match isn't found.
	 */
	Object* getObject(std::string const& symName, NameList const* symParams);
	/**
	 * Searches for a sort matching the given name in the
	 * translator's data structures.
	 * @param symName - The base name of the sort to find.
	 * @return A pointer to the first matching Sort object, or NULL
	 * if a match isn't found.
	 */
	Sort* getSort(std::string const& symName);

	/**
	 * Searches for a variable matching the given name in the
	 * translator's data structures.
	 * @param symName - The base name of the variable to find.
	 * @return A pointer to the first matching Variable object, or NULL
	 * if a match isn't found.
	 */
	Variable* getVariable(std::string const& symName);
	/**
	 * Searches for a stored query with the given label in the translator's
	 * data structures.
	 * @param testLabel - The label of the query to find.
	 * @return A pointer to the first matching query with the given label, or
	 * NULL if a match isn't found.
	 */
	Query* getQuery(std::string const& testLabel);
	
	/**
	 * Searches for a constant like one matching the given name and number of parameters in the
	 * translator's data structures. Used to try to connect a constant-like
	 * element to an actual constant instance.
	 * @param symName - The base name of the constant to find.
	 * @param numParams - The number of parameters the target constant should have.
	 * @return A pointer to the first matching Constant object, or NULL
	 * if a match isn't found.
	 */
	Constant* getConstantLike(std::string const& symName, size_t numParams);

	/**
	 * Searches for an object like one matching the given name and parameters in the
	 * translator's data structures. Used to try to connect an object-like
	 * element to an actual object instance.
	 * @param symName - The base name of the object to find.
	 * @param numParams - A number of parameters the target object should have.
	 * @param isLua - Whether the object is really a LUA object...
	 * @return A pointer to the first matching Object instance, or NULL
	 * if a match isn't found.
	 */
	Object* getObjectLike(std::string const& symName, size_t numParams, bool isLua = false);


	/**
	 * Helper function for getting an Object type reference or dynamically adding it
	 * if it doesn't occur.
	 * @param symName - The base name of the object to find.
	 * @param type - The type of object that the should be created.
	 * @return A pointer to the first matching Object instance (or the newly created one if none were found)
	 */
	Object* getOrCreateSimpleObjectLike(std::string const& symName,  Object::ObjectType type = Object::OBJ_NAME);

	/**
	 * Searches for a sort like one matching the given name in the
	 * translator's data structures. Used to try to connect a sort-like
	 * element to an actual sort instance.
	 * @param symName - The base name of the sort to find.
	 * @return A pointer to the first matching Sort object, or NULL
	 * if a match isn't found.
	 */
	Sort* getSortLike(std::string const& symName);
	/**
	 * Searches for a variable like one matching the given name in the
	 * translator's data structures. Used to try to connect a variable-like
	 * element to an actual variable instance.
	 * @param symName - The base name of the variable to find.
	 * @return A pointer to the first matching Variable object, or NULL
	 * if a match isn't found.
	 */
	Variable* getVariableLike(std::string const& symName);
	
	/* Methods to add elements into the translator's data structures. */
	
	/**
	 * Attempts to add a new Constant object to the translator's data structures and the symbol table.
	 * The object must not already be present in the symbol table, or it will be flagged as a duplicate.
	 * @param newConst - A populated Constant instance to add.
	 * @return A value from SymbolTable's SymTblResult enum indicating success/failure of the add.
	 */
	int addConstant(Constant* newConst);
	/**
	 * Adds a new Object instance into the translator's data structures.
	 * Does not check for duplicates, and does not add the object to the symbol table.
	 * @param newObj - A populated Object instance to add.
	 * @return A value from SymbolTable's SymTblResult enum indicating success/failure of the add.
	 */
	int addObject(Object* newObj);

	/**
	 * Attempts to add a new Sort object to the translator's data structures and the symbol table.
	 * The object must not already be present in the symbol table, or it will be flagged as a duplicate.
	 * @param newSort - A populated Sort instance to add.
	 * @return A value from SymbolTable's SymTblResult enum indicating success/failure of the add.
	 */
	int addSort(Sort* newSort);

	/**
	 * Attempts to create and add a sort object to the translator.
	 * @param sortName The name of the sort to add.
	 * @param subsorts A (possibly NULL) list of subsorts to attach to the sort. In the event the sort already exists, these subsorts will be added to the existing sort.
	 * @param translateDeclaration Whether to add the declaration to the translation output.
	 * @param warnOnDup Whether to print a warning if the sort declaration is a duplicate.
	 * @return A pointer to the newly created sort or the previously existing sort.
	 */

	Sort* addSort(std::string const& sortName, SortList* subsorts = NULL, bool translateDeclaration = true, bool warnOnDup = false);

	/**
	 * Attempts to add a new Variable object to the translator's data structures and the symbol table.
	 * The object must not already be present in the symbol table, or it will be flagged as a duplicate.
	 * @param newVar - A populated Variable instance to add.
	 * @return A value from SymbolTable's SymTblResult enum indicating success/failure of the add.
	 */
	int addVariable(Variable* newVar);
	/**
	 * Attempts to add a new Query object to the translator's data structures.
	 * A query with the same label must not already be present, or it will be flagged as a duplicate.
	 * @param newQuery - A populated Query instance to add.
	 * @return A value from SymbolTable's SymTblResult enum indicating success/failure of the add.
	 */
	int addQuery(Query* newQuery);
	
	/* Methods that translate elements into ASP code. */
	
	/**
	 * Translates a Constant element into an ASP-compatible constant declaration.
	 * Sends the result to ostOut.
	 * @param transConst - The Constant element to translate.
	 */
	void translateConstantDecl(Constant const* transConst);
	/**
	 * Translates an Object element into an ASP-compatible declaration of an
	 * object as a member of a sort's domain.
	 * Sends the result to ostOut.
	 * If the sort pointer is NULL, will not output anything.
	 * @param transObj - The Object element to translate.
	 * @param sortObj - The Sort element the object belongs to.
	 */
	void translateObjectDecl(Object const* transObj, Sort const* sortObj);
	/**
	 * Translates a Sort element into an ASP-compatible sort declaration.
	 * Sends the result to ostOut.
	 * @param transSort - The Sort element to translate.
	 */
	void translateSortDecl(Sort const* transSort);
	/**
	 * Translates a declaration of a super/sub-sort relationship into
	 * ASP-compatible syntax.
	 * Assumes the sorts are declared elsewhere.
	 * @param transSupersort - The "parent" sort object.
	 * @param transSubsort - The "child" sort object.
	 */
	void translateSubsortDecl(Sort const* transSupersort, Sort const* transSubsort);
	/**
	 * Translates a Variable element into an ASP-compatible variable declaration.
	 * Sends the result to ostOut.
	 * If the variable's sort reference is NULL, will not output anything.
	 * @param transVar - The Variable element to translate.
	 */
	void translateVariableDecl(Variable const* transVar);
	/**
	 * Translates a Query into an ASP-compatible query declaration.
	 * Sends the result to ostOut.
	 * Also ensures the standard query label linking statement has been output.
	 * If the query's label or maxstep are blank, will not output anything.
	 * @param transQuery - The Query element to translate.
	 */
	void translateQuery(Query const* transQuery);
	
	/**
	 * Translates a causal law into ASP-compatible rule(s). Sends the result to ostOut.
	 * This models a causal law of the basic form "caused head if ifBody after afterBody unless unlessBody when whenBody following followingBody where whereBody."
	 * Passing NULL for the various parameters will change what kind of translation is performed,
	 * and the type of elements (if any) in the parameters will also affect the translation.
	 * @param head - The head portion of the causal law ("caused head...").
	 * @param ifBody - The part of the law's body associated with the if keyword ("...if ifBody...").
	 * @param assumingBody - The part of the law's body associated with the if keyword ("...assuming ifBody...").
	 * @param afterBody - The part of the law's body associated with the after keyword ("...after afterBody...").
	 * @param unlessBody - The part of the law's body associated with the unless keyword ("...unless unlessBody...").
	 * @param whenBody - The part of the law's body associated with the when keyword ("...when whenBody...").
	 * @param followingBody - The part of the law's body associated with the following keyword ("...following followingBody...").
	 * @param whereBody - The part of the law's body associated with the where keyword ("...where whereBody.").
	 */
	void translateCausalLaw(
		ParseElement* head,
		ParseElement* ifBody,
		ParseElement* assumingBody,
		ParseElement* afterBody,
		ParseElement* unlessBody,
		ParseElement* whenBody,
		ParseElement* followingBody,
		ParseElement* whereBody
		);

	/**
	 * Helper wrapper to translateCausalLaw w/o an unlessBody
	 * @param head - The head portion of the causal law ("caused head...").
	 * @param ifBody - The part of the law's body associated with the if keyword ("...if ifBody...").
	 * @param assumingBody - The part of the law's body associated with the if keyword ("...assuming ifBody...").
	 * @param afterBody - The part of the law's body associated with the after keyword ("...after afterBody...").
	 * @param whenBody - The part of the law's body associated with the when keyword ("...when whenBody...").
	 * @param followingBody - The part of the law's body associated with the following keyword ("...following followingBody...").
	 * @param whereBody - The part of the law's body associated with the where keyword ("...where whereBody.").
	 */
	inline void translateCausalLaw(
		ParseElement* head,
		ParseElement* ifBody,
		ParseElement* assumingBody,
		ParseElement* afterBody,
		ParseElement* whenBody,
		ParseElement* followingBody,
		ParseElement* whereBody
		) { translateCausalLaw(head, ifBody, assumingBody, afterBody, NULL, whenBody, followingBody, whereBody); }
	

	/**
	 * Handles a ':- show' statement, adding the appropriate #show statements to the program.
	 * @param atomicFormulas The list of atomic formulas which were included in the show statement.
	 */
	void translateShowStmt(ParseElementList const& atomicFormulas);


	/**
	 * Transforms declarative laws ("inertial p", "exogenous a(X)", "rigid q", etc.) to basic form and calls the translator for them.
	 * @param head - The head of the law, in this case a bare constant-like expression.
	 * @param ifBody - Optional conditional formula to govern when the law applies.
	 * @param assumingBody - Optional conditional formula similar to the 'ifBody', except results in edges in the dependency graph.
	 * @param afterBody - Optional conditional formula specifying conditions from the prior time step.
	 * @param unlessBody -  Optional formula that acts as an abnormality condition.
	 * @param whenBody - Optional conditional formula specifying abnormalities in the current time step.
	 * @param followingBody - Optional conditional formula specifying abnormalities in the previous time step.
	 * @param whereBody - Another conditional formula to govern when the law applies.
	 * @param opType - The kind of declaration this is (exogenous, inertial, etc.).
	 * @return True if everything translates properly, false if anything goes wrong.
	 */
	bool translateDeclarativeLaw(
		ParseElement* head,
		ParseElement* ifBody,
		ParseElement* assumingBody,
		ParseElement* afterBody,
		ParseElement* unlessBody,
		ParseElement* whenBody,
		ParseElement* followingBody,
		ParseElement* whereBody,
		SimpleUnaryOperator::UnaryOperatorType opType
	);

	/**
	 * Transforms a causal law of the form "always F [where G]." to basic form, then calls the translator for it.
	 * @param constraint - The condition that must be true.
	 * @param whenBody - Optional conditional formula specifying abnormalities in the current time step.
	 * @param whereBody - A conditional formula to govern when the law applies.
	 * @return True if everything translates properly, false if anything goes wrong.
	 */
	bool translateAlwaysLaw(
		ParseElement* constraint,
		ParseElement* whenBody,
		ParseElement* whereBody
	);

	/**
	 * Transforms a causal law of the form "constraint F [after H] [where J]." to basic form, then calls the translator for it.
	 * @param constraint - The condition that must be true.
	 * @param afterBody - Optional conditional formula specifying restrictions from the prior time step.
	 * @param unlessBody - Optional formula that acts as an abnormality condition.
	 * @param whenBody - Optional conditional formula specifying abnormalities in the current time step.
	 * @param followingBody - Optional conditional formula specifying abnormalities in the previous time step.
	 * @param whereBody - Another conditional formula to govern when the law applies.
	 * @param positive - Whether the constraint is of the positive 'constraint F' form, or negative 'impossible F' form.
	 * @return True if everything translates properly, false if anything goes wrong.
	 */
	bool translateConstraintLaw(
		ParseElement* constraint,
		ParseElement* afterBody,
		ParseElement* unlessBody,
		ParseElement* whenBody,
		ParseElement* followingBody,
		ParseElement* whereBody,
		bool positive
	);

	/**
	 * Transforms a causal law of the form "default F [if G] [after H] [where J]." to basic form, then calls the translator on it.
	 * @param head - The head of the law.
	 * @param ifBody - Optional conditional formula to govern when the law applies.
	 * @param assumingBody - Optional conditional formula similar to the 'ifBody', except results in edges in the dependency graph.
	 * @param afterBody - Optional conditional formula specifying conditions from the prior time step.
	 * @param unlessBody - Optional atom to be dynamically declared as an default-false constant.
	 * @param whenBody - Optional conditional formula specifying abnormalities in the current time step.
	 * @param followingBody - Optional conditional formula specifying abnormalities in the previous time step.
	 * @param whereBody - Another conditional formula to govern when the law applies.
	 * @return True if everything translates properly, false if anything goes wrong.
	 */
	bool translateDefaultLaw(
		ParseElement* head,
		ParseElement* ifBody,
		ParseElement* assumingBody,
		ParseElement* afterBody,
		ParseElement* unlessBody,
		ParseElement* whenBody,
		ParseElement* followingBody,
		ParseElement* whereBody
	);

	/**
	 * Transforms a causal law of the form "nonexecutable F [if G] [where H]." to basic form, then calls the translator for it.
	 * @param nonEx - The formula that should not be executed.
	 * @param ifBody - Optional conditional formula to govern when the law applies.
	 * @param whenBody - Optional conditional formula specifying abnormalities in the current time step.
	 * @param whereBody - Another conditional formula to govern when the law applies.
	 * @return True if everything translates properly, false if anything goes wrong.
	 */
	bool translateNonexecutableLaw(
		ParseElement* nonEx,
		ParseElement* ifBody,
		ParseElement* whenBody,
		ParseElement* whereBody
	);

	/**
	 * Transforms a causal law of the form "possibly caused F [if G] [after H] [where J]." to basic form, then calls the translator on it.
	 * @param head - The head of the law.
	 * @param ifBody - Optional conditional formula to govern when the law applies.
	 * @param assumingBody - Optional conditional formula similar to the 'ifBody', except results in edges in the dependency graph.
	 * @param afterBody - Optional conditional formula specifying conditions from the prior time step.
	 * @param unlessBody - Optional atom to be dynamically declared as an default-false constant.
	 * @param whenBody - Optional conditional formula specifying abnormalities in the current time step.
	 * @param followingBody - Optional conditional formula specifying abnormalities in the previous time step.
	 * @param whereBody - Another conditional formula to govern when the law applies.
	 * @return True if everything translates properly, false if anything goes wrong.
	 */
	bool translatePossiblyCausedLaw(
		ParseElement* head,
		ParseElement* ifBody,
		ParseElement* assumingBody,
		ParseElement* afterBody,
		ParseElement* unlessBody,
		ParseElement* whenBody,
		ParseElement* followingBody,
		ParseElement* whereBody
	);

	/**
	 * Transforms a causal law of the form "G may cause F [if H] [where J]." to basic form, then calls the translator on it.
	 * @param causer - The causing action formula.
	 * @param causee - The formula being caused.
	 * @param ifBody - Optional conditional formula to govern when the law applies.
	 * @param whenBody - Optional conditional formula specifying abnormalities in the current time step.
	 * @param whereBody - Another conditional formula to govern when the law applies.
	 * @return True if everything translates properly, false if anything goes wrong.
	 */
	bool translateMayCauseLaw(
		ParseElement* causer,
		ParseElement* causee,
		ParseElement* ifBody,
		ParseElement* whenBody,
		ParseElement* whereBody
	);

	/**
	 * Transforms a causal law of the form "G causes F [if H] [where J]." to basic form, then calls the translator on it.
	 * @param causer - The causing action formula.
	 * @param causee - The formula being caused.
	 * @param ifBody - Optional conditional formula to govern when the law applies.
	 * @param unlessBody - Optional atom to be dynamically declared as an default-false constant.
	 * @param whenBody - Optional conditional formula specifying abnormalities in the current time step.
	 * @param whereBody - Another conditional formula to govern when the law applies.
	 * @return True if everything translates properly, false if anything goes wrong.
	 */
	bool translateCausesLaw(
		ParseElement* causer,
		ParseElement* causee,
		ParseElement* ifBody,
		ParseElement* unlessBody,
		ParseElement* whenBody,
		ParseElement* whereBody
	);

	/**
	 * Transforms a causal law of the form "A increments B by N [if H] [where J]." to basic form, then calls the translator on it.
	 * @param causer - The causing action.
	 * @param causee - The additive constant being incremented.
	 * @param increment - The increment expression
	 * @param ifBody - Optional conditional formula to govern when the law applies.
	 * @param whenBody - Optional conditional formula specifying abnormalities in the current time step.
	 * @param whereBody - Another conditional formula to govern when the law applies.
	 * @param isIncrement - True if the law is increment, false if it is decrement.
	 * @return True if everything translates properly, false if anything goes wrong.
	 */
	bool translateIncrementLaw(
		ParseElement* causer,
		ParseElement* causee,
		ParseElement* increment,
		ParseElement* ifBody,
		ParseElement* whenBody,
		ParseElement* whereBody,
		bool isIncrement
	);


	/**
	 * Appends any statements that have been witheld for the footer to the translated program.
	 */
	inline void outputFooter() { outputStmts(mFooterStmts); }


	/* Translator-specific methods for general setup etc. */
	
	/**
	 * Allocates and adds a sort with the given name to the translator's
	 * internal data structures without translating or outputting it.
	 * Also sets up a default Variable for the sort.
	 * @param newSortName - The base name for the new sort.
	 * @param subsorts - A list of Sort pointers to sorts that are subsorts of
	 * of the sort to be created. NULL if there are no subsorts.
	 * @return A pointer to the newly created and added sort, or NULL on an error.
	 */
	inline Sort* createInternalSort(std::string const& newSortName, SortList* subsorts = NULL)
		{ return addSort(newSortName, subsorts, false); }
	
	/**
	 * Allocates and adds an object with the given name to the translator's
	 * internal data structures, connecting it as a member of the given sort's domain.
	 * @param newObjName - The base name for the new object.
	 * @param params - A list of Sort pointers representing the parameters of the object. NULL if there are no params.
	 * @param ofSort - The sort the new object is a member of.
	 * @return A pointer to the newly created and added object, or NULL on an error.
	 */
	Object* createInternalObject(std::string const& newObjName, SortList* params, Sort* ofSort);
	
	/**
	 * Allocates and adds a number range with the given name and bounds to the translator's
	 * internal data structures, connecting it as a member of the given sort's domain.
	 * @param newObjName - The base name for the new object.
	 * @param ofSort - The sort the new object is a member of.
	 * @return A pointer to the newly created and added number range, or NULL on an error.
	 */
	NumberRange* createInternalNumRange(std::string const& newObjName, Sort* ofSort);
	
	/**
	 * Allocates a new Query instance and assigns it to tempQuery.
	 * @param deallocateFirst - If true, will deallocate the contents of tempQuery before the reallocation. (Default: false)
	 */
	void allocateNewTempQuery(bool deallocateFirst=false);
	
	/**
	 * Sets a translator instance's output stream to the passed stream.
	 * @param newOst - The output stream the translator should use to output
	 * its translations.
	 */
	void setOutput(std::ostream &newOst);
	
	/**
	 * Sets a translator instance's error output stream to the passed stream.
	 * @param newOst - The output stream the translator module should use when
	 * reporting errors or other problems.
	 */
	void setErrorOutput(std::ostream &newOst);
	
	/**
	 * Sets whether the translator instance should output a static translation
	 * of the CCalc source file instead of an incremental/dynamic translation.
	 * @param staticTrans - Whether a static translation should be used.
	 */
	void setStaticTranslation(bool staticTrans);

	/**
	 * Outputs the contents of str to wherever the translator's error output stream is aimed.
	 * Mirrors output to the standard output stream as well.
	 * @param str - The text to output.
	 * @param endWithNewline - Whether to terminate the message in a newline.
	 * @param location - Whether to include the parser's current location in the message.
	 */
	void error(std::string const& str, bool endWithNewline=false, bool location = true);

	/**
	 * Outputs the contents of str to wherever the translator's error output stream is aimed.
	 * Mirrors output to the standard output stream as well.
	 * @param str - The text to output.
	 */
	void warn(std::string const& str, bool endWithNewline=false, bool location = true);

	/**
	 * Outputs the contents of str to wherever the translator's error output stream is aimed.
	 * Mirrors output to the standard output stream as well.
	 * @param str - The text to output.
	 */
	void caution(std::string const& str, bool endWithNewline=false, bool location = true);

	/**
	 * Outputs the contents of str to wherever the translator's error output stream is aimed.
	 * Mirrors output to the standard output stream as well.
	 * @param str - The text to output.
	 */
	void pragma(std::string const& str, bool endWithNewline=false, bool location = true);


	/**
	 * Destructor. Deallocates all memory associated with the objects in the
	 * list attributes and clears their contents.
	 */
	virtual ~Translator();

	/**
	 * helper method to output multiple clauses to an output stream.
	 * @param out - The stream to output to.
	 * @param clauses - The clauses to print.
	 * @param initConj - Whether to output an initial conjunction symbol before the clauses.
	 * @return The output stream.
	 */
	static std::ostream& outputClauses(std::ostream& out, ClauseList const& clauses, bool initConj = false);

	/**
	 * helper method to translate a sub expression, capturing all of its clauses and variables in the meanwhile.
	 * @param out - The stream to output to.
	 * @param expr - The expression to translate.
	 * @param context - The current translation context.
	 * @param upwardMobileClauses - Whether we should migrate clauses to the parent context if no quantification was needed.
	 * @param suppressQuantifier - Whether to suppress the generation of quantifiers generated when leaving the context. Free variables will be simply dropped.
	 * @return The output stream.
	 */
	static std::ostream& bindAndTranslate(std::ostream& out, ParseElement const* expr, Context& context, bool upwardMobileClauses, bool suppressQuantifier = false);


	/**
	 * Outputs the contents of str to wherever the translator's output stream is aimed.
	 * @param str - The text to output.
	 * @param incPart - The Incremental part that the chunk is associated with.
	 * @param endWithNewline - If true, will append a newline character to the end of str.
	 */
	void output(std::string const& str, IPart incPart, bool endWithNewline=false);

	/**
	 * Outputs a list of statements to the translators output.
	 * @param stmts - The statements to print.
	 */
	void outputStmts(StmtList const& stmts);


	/**
	 * Appends the provided statement to the program's footer buffer, resulting them in being included in the footer in the future.
	 * @param stmts - The statements to be added to the footer. Will be emptied by the operation.
	 */
	void addToFooter(StmtList& stmts) { mFooterStmts.splice(mFooterStmts.end(), stmts); }

	/**
	 * Instructs the translator to output its symbol table to the specified location.
	 * @param out The output stream to dump the symbol table to.
	 */
	std::ostream& outputSymbolTable(std::ostream& out);

protected:

	/**
	 * Sets incremental part to the provided part, outputting the appropriate clause
	 * to the translation output.
	 * @param newIncPart - The new incremental module to work with.
	 * @return True if successful, false otherwise.
	 */
	bool setIncrementalPart(IPart newIncPart);

	/**
	 * Outputs the contents of str to wherever the translator's output stream is aimed.
	 * @param str - The text to output.
	 * @param endWithNewline - If true, will append a newline character to the end of str.
	 */
	void output(std::string const& str, bool endWithNewline=false);

	/**
	 * Outputs the contents of hdr followed by str to wherever the translator's error output stream is aimed.
	 * Mirrors output to the standard output stream as well.
	 * @param str - The text to output.
	 */
	void outputToErr(std::string const& hdr, std::string const& str, bool endWithNewline, bool location);

	/**
	 * A helper function for translating causal laws. Called by translateCausalLaw
	 * @param output[out] - The output stream to write the generated translation to.
	 * @param extraStmts[out] - A list of extra statments generated by translating this law.
	 * @param ipart - The incremental part that this law belongs to.
	 * @param ifNotNot - Whether the rule's ifBody needs to be wrapped in double negation.
	 * @param afterNotNot - Whether the rule's afterBody needs to be wrapped in double negation.
	 * @param baseTimeStamp - The timestamp to use for the translated law.
	 * @param head - The head portion of the causal law ("caused head...").
	 * @param ifBody - The part of the law's body associated with the if keyword ("...if ifBody...").
	 * @param assumingBody - The part of the law's body associated with the if keyword ("...assuming ifBody...").
	 * @param unlessBody - The part of the law's body associated with the unless keyword ("...unless unlessBody...").
	 * @param afterBody - The part of the law's body associated with the after keyword ("...after afterBody...").
	 * @param whenBody - The part of the law's body associated with the when keyword ("...when whenBody...").
	 * @param followingBody - The part of the law's body associated with the following keyword ("...following followingBody...").
	 * @param whereBody - The part of the law's body associated with the where keyword ("...where whereBody.").
	 * @return
	 */
	static std::ostream& makeCausalTranslation(
		std::ostream& output,
		StmtList& extraStmts,
		IPart ipart,
		bool ifNotNot,
		bool afterNotNot,
		std::string const& baseTimeStamp,
		ParseElement* head,
		ParseElement* ifBody,
		ParseElement* assumingBody,
		ParseElement* unlessBody,
		ParseElement* afterBody,
		ParseElement* whenBody,
		ParseElement* followingBody,
		ParseElement* whereBody
		);

};

#endif // TRANSLATOR_H
