/*
* Copyright 2026 Rochus Keller <mailto:me@rochus-keller.ch>
*
* This file is part of the Simula67 parser library.
*
* The following is the license that applies to this copy of the
* library. For a license to use the library under conditions
* other than those described here, please email to me@rochus-keller.ch.
*
* GNU General Public License Usage
* This file may be used under the terms of the GNU General Public
* License (GPL) versions 2.0 or 3.0 as published by the Free Software
* Foundation and appearing in the file LICENSE.GPL included in
* the packaging of this file. Please review the following information
* to ensure GNU General Public Licensing requirements will be met:
* http://www.fsf.org/licensing/licenses/info/GPLv2.html and
* http://www.gnu.org/copyleft/gpl.html.
*/

#include "SimCeeGen.h"
#include <QFile>
#include <QFileInfo>
#include <QtDebug>

using namespace Sim;

CeeGen::CeeGen()
    : indentLevel(0)
    , tempVarCounter(0)
    , labelCounter(0)
    , currentModule(0)
    , currentClass(0)
    , currentProc(0)
{
}

CeeGen::~CeeGen()
{
}

void CeeGen::error(const RowCol& pos, const QString& msg)
{
    errors << Error(msg, pos);
}

QString CeeGen::indent() const
{
    return QString(indentLevel * 4, ' ');
}

QString CeeGen::newTempVar(const QString& prefix)
{
    return QString("%1_%2").arg(prefix).arg(++tempVarCounter);
}

QString CeeGen::newLabel(const QString& prefix)
{
    return QString("%1_%2").arg(prefix).arg(++labelCounter);
}

// ============================================================================
// Name Mangling
// ============================================================================

QString CeeGen::mangleName(Declaration* d)
{
    if (!d)
        return "NULL";
    
    if (mangledNames.contains(d))
        return mangledNames[d];
    
    QString result;
    switch (d->kind) {
    case Declaration::Class:
    case Declaration::StandardClass:
        result = mangleClassName(d);
        break;
    case Declaration::Procedure:
        result = mangleProcName(d);
        break;
    case Declaration::Variable:
    case Declaration::Parameter:
    case Declaration::Array:
        result = mangleVarName(d);
        break;
    default:
        result = QString("sim_%1").arg(d->name.constData());
        break;
    }
    
    mangledNames[d] = result;
    return result;
}

QString CeeGen::mangleClassName(Declaration* cls)
{
    if (!cls)
        return "SimObject";
    
    QString name = cls->name.constData();
    
    // Handle standard classes
    if (name.toLower() == "environment")
        return "SimEnvironment";
    if (name.toLower() == "basicio")
        return "SimBasicIO";
    if (name.toLower() == "simset")
        return "SimSimset";
    if (name.toLower() == "simulation")
        return "SimSimulation";
    if (name.toLower() == "file")
        return "SimFile";
    if (name.toLower() == "imagefile")
        return "SimImageFile";
    if (name.toLower() == "infile")
        return "SimInFile";
    if (name.toLower() == "outfile")
        return "SimOutFile";
    if (name.toLower() == "printfile")
        return "SimPrintFile";
    if (name.toLower() == "directfile")
        return "SimDirectFile";
    if (name.toLower() == "linkage")
        return "SimLinkage";
    if (name.toLower() == "link")
        return "SimLink";
    if (name.toLower() == "head")
        return "SimHead";
    if (name.toLower() == "process")
        return "SimProcess";
    
    // User-defined class
    return QString("Sim_%1").arg(name);
}

QString CeeGen::mangleProcName(Declaration* proc)
{
    if (!proc)
        return "sim_unknown_proc";
    
    QString name = proc->name.constData();
    
    // If procedure is a class member, include class name
    if (proc->outer && (proc->outer->kind == Declaration::Class || 
                        proc->outer->kind == Declaration::StandardClass)) {
        return QString("%1_%2").arg(mangleClassName(proc->outer)).arg(name);
    }
    
    return QString("sim_%1").arg(name);
}

QString CeeGen::mangleVarName(Declaration* var)
{
    if (!var)
        return "sim_unknown_var";
    
    QString name = var->name.constData();
    
    // Avoid C keywords
    static QSet<QString> cKeywords;
    if (cKeywords.isEmpty()) {
        cKeywords << "auto" << "break" << "case" << "char" << "const"
                  << "continue" << "default" << "do" << "double" << "else"
                  << "enum" << "extern" << "float" << "for" << "goto"
                  << "if" << "int" << "long" << "register" << "return"
                  << "short" << "signed" << "sizeof" << "static" << "struct"
                  << "switch" << "typedef" << "union" << "unsigned" << "void"
                  << "volatile" << "while" << "inline" << "restrict" << "_Bool"
                  << "_Complex" << "_Imaginary";
    }
    
    if (cKeywords.contains(name.toLower()))
        return QString("sim_%1").arg(name);
    
    return name;
}

QString CeeGen::mangleTypeName(Type* t)
{
    if (!t)
        return "void";
    return mapType(t);
}

// ============================================================================
// Type Mapping
// ============================================================================

QString CeeGen::mapType(Type* t)
{
    if (!t)
        return "void";
    
    switch (t->kind) {
    case Type::Integer:
        return "int32_t";
    case Type::ShortInteger:
        return "int16_t";
    case Type::Real:
        return "double";
    case Type::LongReal:
        return "long double";
    case Type::Boolean:
        return "bool";
    case Type::Character:
        return "char";
    case Type::Text:
        return "SimText";
    case Type::Label:
        return "SimLabel";
    case Type::Ref: {
        Declaration* refType = t->getRefType();
        if (refType)
            return QString("%1*").arg(mangleClassName(refType));
        return "SimObject*";
    }
    case Type::Array:
        return "SimArray*";
    case Type::Procedure:
        return "SimProcRef";
    case Type::Switch:
        return "SimSwitch*";
    case Type::None:
        return "void*";
    case Type::Notext:
        return "SimText";
    default:
        return "void";
    }
}

QString CeeGen::mapBasicType(Type::Kind k)
{
    switch (k) {
    case Type::Integer:
        return "int32_t";
    case Type::ShortInteger:
        return "int16_t";
    case Type::Real:
        return "double";
    case Type::LongReal:
        return "long double";
    case Type::Boolean:
        return "bool";
    case Type::Character:
        return "char";
    case Type::Text:
        return "SimText";
    default:
        return "void";
    }
}

bool CeeGen::isBuiltinDecl(Declaration* d)
{
    if (!d)
        return false;
    
    if (d->kind == Declaration::Builtin || d->kind == Declaration::StandardClass)
        return true;
    
    // Check if it's in the Environment or BasicIO scope
    Declaration* outer = d->outer;
    while (outer) {
        QString name = outer->name.constData();
        if (name.toLower() == "environment" || name.toLower() == "basicio" ||
            name.toLower() == "simset" || name.toLower() == "simulation")
            return true;
        outer = outer->outer;
    }
    
    return d->isExternal;
}

bool CeeGen::isBuiltinProc(Declaration* d)
{
    if (!d || d->kind != Declaration::Procedure)
        return false;
    
    return isBuiltinDecl(d);
}

QString CeeGen::getBuiltinProcName(Declaration* d)
{
    if (!d)
        return "";
    
    QString name = QString(d->name.constData()).toLower();
    
    // Character handling
    if (name == "char") return "sim_char";
    if (name == "isochar") return "sim_isochar";
    if (name == "rank") return "sim_rank";
    if (name == "isorank") return "sim_isorank";
    if (name == "digit") return "sim_digit";
    if (name == "letter") return "sim_letter";
    if (name == "upcase") return "sim_upcase";
    if (name == "lowcase") return "sim_lowcase";
    
    // Text handling
    if (name == "blanks") return "sim_blanks";
    if (name == "copy") return "sim_copy";
    
    // Math
    if (name == "abs") return "sim_abs";
    if (name == "sqrt") return "sqrt";
    if (name == "sin") return "sin";
    if (name == "cos") return "cos";
    if (name == "tan") return "tan";
    if (name == "cotan") return "sim_cotan";
    if (name == "arcsin") return "asin";
    if (name == "arccos") return "acos";
    if (name == "arctan") return "atan";
    if (name == "exp") return "exp";
    if (name == "ln") return "log";
    if (name == "sign") return "sim_sign";
    if (name == "entier") return "sim_entier";
    if (name == "mod") return "fmod";
    
    // System
    if (name == "error") return "sim_error";
    if (name == "time") return "sim_time";
    if (name == "cputime") return "sim_cputime";
    if (name == "sourceline") return "sim_sourceline";
    
    // Random
    if (name == "draw") return "sim_draw";
    if (name == "randint") return "sim_randint";
    if (name == "uniform") return "sim_uniform";
    if (name == "normal") return "sim_normal";
    if (name == "negexp") return "sim_negexp";
    if (name == "poisson") return "sim_poisson";
    if (name == "erlang") return "sim_erlang";
    if (name == "discrete") return "sim_discrete";
    if (name == "histd") return "sim_histd";
    if (name == "linear") return "sim_linear";
    
    // Array
    if (name == "lowerbound") return "sim_lowerbound";
    if (name == "upperbound") return "sim_upperbound";
    
    // I/O wrappers
    if (name == "inimage") return "sim_inimage";
    if (name == "outimage") return "sim_outimage";
    if (name == "inint") return "sim_inint";
    if (name == "inreal") return "sim_inreal";
    if (name == "infrac") return "sim_infrac";
    if (name == "inchar") return "sim_inchar";
    if (name == "intext") return "sim_intext";
    if (name == "lastitem") return "sim_lastitem";
    if (name == "outint") return "sim_outint";
    if (name == "outreal") return "sim_outreal";
    if (name == "outfix") return "sim_outfix";
    if (name == "outfrac") return "sim_outfrac";
    if (name == "outchar") return "sim_outchar";
    if (name == "outtext") return "sim_outtext";
    if (name == "setpos") return "sim_setpos";
    if (name == "pos") return "sim_pos";
    if (name == "eject") return "sim_eject";
    if (name == "line") return "sim_line";
    
    // Coroutines
    if (name == "detach") return "sim_detach";
    if (name == "resume") return "sim_resume";
    if (name == "call") return "sim_call";
    if (name == "terminate_program") return "sim_terminate_program";
    
    return QString("sim_%1").arg(name);
}

// ============================================================================
// Main Entry Point
// ============================================================================

bool CeeGen::transpile(Declaration* module, const QString& outputPath)
{
    if (!module) {
        error(RowCol(), "No module to transpile");
        return false;
    }
    
    errors.clear();
    generatedCode.clear();
    headerCode.clear();
    forwardDecls.clear();
    structDefs.clear();
    vtableDefs.clear();
    funcDefs.clear();
    mainCode.clear();
    
    indentLevel = 0;
    tempVarCounter = 0;
    labelCounter = 0;
    
    emittedClasses.clear();
    emittedProcs.clear();
    classIdMap.clear();
    mangledNames.clear();
    
    currentModule = module;
    
    // Generate code
    emitModule(module);
    
    if (!errors.isEmpty())
        return false;
    
    // Assemble final output
    QTextStream out(&generatedCode);
    
    emitIncludes(out);
    out << "\n";
    emitRuntimeDecls(out);
    out << "\n";
    out << forwardDecls;
    out << "\n";
    out << structDefs;
    out << "\n";
    out << vtableDefs;
    out << "\n";
    out << funcDefs;
    out << "\n";
    out << mainCode;
    
    // Write to file if path provided
    if (!outputPath.isEmpty()) {
        QFile file(outputPath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            error(RowCol(), QString("Cannot open output file: %1").arg(outputPath));
            return false;
        }
        QTextStream fileOut(&file);
        fileOut << generatedCode;
        file.close();
    }
    
    return errors.isEmpty();
}

void CeeGen::emitIncludes(QTextStream& out)
{
    out << "/* Generated by SimTranspiler - Simula 67 to C99 */\n";
    out << "#include <stdio.h>\n";
    out << "#include <stdlib.h>\n";
    out << "#include <stdint.h>\n";
    out << "#include <stdbool.h>\n";
    out << "#include <string.h>\n";
    out << "#include <math.h>\n";
    out << "#include <gc.h>\n";
    out << "#include \"sim_runtime.h\"\n";
}

void CeeGen::emitRuntimeDecls(QTextStream& out)
{
    out << "/* Forward declarations */\n";
    out << "typedef struct SimObject SimObject;\n";
    out << "typedef struct SimText SimText;\n";
    out << "typedef struct SimArray SimArray;\n";
    out << "typedef struct SimLabel SimLabel;\n";
    out << "typedef struct SimSwitch SimSwitch;\n";
    out << "typedef void* SimProcRef;\n";
    out << "\n";
}

// ============================================================================
// Module Generation
// ============================================================================


void CeeGen::emitModule(Declaration* mod)
{
    if (!mod)
        return;
    
    // Collect forward declarations
    collectForwardDecls(mod);
    
    Declaration* d = mod->link;
    while( d )
    {
        // TODO: assure that there is only one main; either consolidate all programs into one
        // or make a separate function for each and call those in main.

        // Emit all declarations
        emitDeclarations(d);

        // Emit main function if this is a program
        if (d->kind == Declaration::Program) {
            QTextStream out(&mainCode);
            out << "\nint main(int argc, char* argv[]) {\n";
            out << "    GC_INIT();\n";
            out << "    sim_init();\n";
            out << "\n";

            // Emit module body
            if (d->body) {
                indentLevel = 1;
                emitStatementSeq(d->body, out);
                indentLevel = 0;
            }

            out << "\n";
            out << "    sim_cleanup();\n";
            out << "    return 0;\n";
            out << "}\n";
        }
        d = d->next;
    }
}

void CeeGen::emitDeclarations(Declaration *d)
{
    while (d) {
        if( d->kind != Declaration::Block && d->kind != Declaration::LabelDecl )
            // Block and label decls are handled in the corresponding statements
            emitDeclaration(d);
        d = d->next;
    }
}

void CeeGen::collectForwardDecls(Declaration* d)
{
    while (d) {
        if (d->kind == Declaration::Class && !isBuiltinDecl(d)) {
            QTextStream out(&forwardDecls);
            QString className = mangleClassName(d);
            out << "typedef struct " << className << " " << className << ";\n";
        }
        
        // Recurse into members
        if (d->link)
            collectForwardDecls(d->link);
        
        d = d->next;
    }
}

void CeeGen::emitDeclaration(Declaration* d)
{
    if (!d)
        return;
    
    // Skip built-in declarations
    if (isBuiltinDecl(d))
        return;
    
    switch (d->kind) {
    case Declaration::Class:
        emitClass(d);
        break;
    case Declaration::Procedure:
        emitProcedure(d);
        break;
    case Declaration::Variable:
        emitVariable(d);
        break;
    case Declaration::Array:
        emitArray(d);
        break;
    case Declaration::Switch:
        emitSwitch(d);
        break;
    case Declaration::Block:
        emitBlock(d);
        break;
    default:
        break;
    }
}

// ============================================================================
// Class Generation
// ============================================================================

void CeeGen::emitClass(Declaration* cls)
{
    if (!cls || emittedClasses.contains(cls))
        return;
    
    emittedClasses.insert(cls);
    
    // First emit prefix class if any
    if (cls->prefix && !isBuiltinDecl(cls->prefix))
        emitClass(cls->prefix);
    
    currentClass = cls;
    
    emitClassStruct(cls);
    emitClassVtable(cls);
    emitClassConstructor(cls);
    
    // Emit member procedures
    Declaration* member = cls->link;
    while (member) {
        if (member->kind == Declaration::Procedure)
            emitProcedure(member);
        member = member->next;
    }
    
    currentClass = 0;
}

void CeeGen::emitClassStruct(Declaration* cls)
{
    QTextStream out(&structDefs);
    QString className = mangleClassName(cls);
    
    out << "/* Class " << cls->name.constData() << " */\n";
    out << "struct " << className << " {\n";
    
    // If has prefix, embed it first
    if (cls->prefix) {
        QString prefixName = mangleClassName(cls->prefix);
        out << "    " << prefixName << " _base;\n";
    } else {
        out << "    SimObject _base;\n";
    }
    
    // Emit member variables (skip parameters, procedures, virtuals)
    Declaration* member = cls->link;
    
    // First emit parameters as members
    while (member && member->kind == Declaration::Parameter) {
        QString varType = mapType(member->type());
        QString varName = mangleVarName(member);
        out << "    " << varType << " " << varName << ";\n";
        member = member->next;
    }
    
    // Then emit other members
    while (member) {
        if (member->kind == Declaration::Variable) {
            QString varType = mapType(member->type());
            QString varName = mangleVarName(member);
            out << "    " << varType << " " << varName << ";\n";
        } else if (member->kind == Declaration::Array) {
            QString varName = mangleVarName(member);
            out << "    SimArray* " << varName << ";\n";
        }
        member = member->next;
    }
    
    out << "};\n\n";
}

void CeeGen::emitClassVtable(Declaration* cls)
{
    QTextStream out(&vtableDefs);
    QString className = mangleClassName(cls);
    
    // Collect virtual procedures
    QList<Declaration*> virtuals;
    Declaration* member = cls->link;
    while (member) {
        if (member->kind == Declaration::VirtualSpec || 
            (member->kind == Declaration::Procedure && member->isVirtual)) {
            virtuals.append(member);
        }
        member = member->next;
    }
    
    // Also collect inherited virtuals from prefix
    if (cls->prefix) {
        QList<Declaration*> hierarchy;
        collectClassHierarchy(cls->prefix, hierarchy);
        for (int i = 0; i < hierarchy.size(); i++) {
            Declaration* prefixMember = hierarchy[i]->link;
            while (prefixMember) {
                if (prefixMember->kind == Declaration::VirtualSpec) {
                    bool found = false;
                    for (int j = 0; j < virtuals.size(); j++) {
                        if (virtuals[j]->sym == prefixMember->sym) {
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                        virtuals.append(prefixMember);
                }
                prefixMember = prefixMember->next;
            }
        }
    }
    
    if (virtuals.isEmpty() && !cls->prefix) {
        // No vtable needed, use base vtable
        out << "static SimVtable " << className << "_vtable = {\n";
        out << "    .class_id = " << getClassId(cls) << ",\n";
        out << "    .class_name = \"" << cls->name.constData() << "\",\n";
        out << "    .parent_id = 0\n";
        out << "};\n\n";
        return;
    }
    
    // Emit vtable structure
    out << "typedef struct " << className << "_Vtable {\n";
    out << "    SimVtable base;\n";
    
    for (int i = 0; i < virtuals.size(); i++) {
        Declaration* v = virtuals[i];
        QString retType = v->type() ? mapType(v->type()) : "void";
        out << "    " << retType << " (*" << v->name.constData() << ")(" << className << "* self";
        
        // Add parameter types
        Declaration* param = v->link;
        while (param && param->kind == Declaration::Parameter) {
            out << ", " << mapType(param->type());
            param = param->next;
        }
        out << ");\n";
    }
    
    out << "} " << className << "_Vtable;\n\n";
    
    // Emit vtable instance
    out << "static " << className << "_Vtable " << className << "_vtable = {\n";
    out << "    .base = {\n";
    out << "        .class_id = " << getClassId(cls) << ",\n";
    out << "        .class_name = \"" << cls->name.constData() << "\",\n";
    if (cls->prefix)
        out << "        .parent_id = " << getClassId(cls->prefix) << "\n";
    else
        out << "        .parent_id = 0\n";
    out << "    }";
    
    for (int i = 0; i < virtuals.size(); i++) {
        Declaration* v = virtuals[i];
        Declaration* match = findVirtualMatch(cls, v);
        out << ",\n    ." << v->name.constData() << " = ";
        if (match && match->kind == Declaration::Procedure) {
            out << mangleProcName(match);
        } else {
            out << "NULL";
        }
    }
    
    out << "\n};\n\n";
}

void CeeGen::emitClassConstructor(Declaration* cls)
{
    QTextStream out(&funcDefs);
    QString className = mangleClassName(cls);
    
    // Constructor signature
    out << className << "* " << className << "_new(";
    
    // Collect parameters from class hierarchy
    QList<Declaration*> hierarchy;
    collectClassHierarchy(cls, hierarchy);
    
    QList<Declaration*> allParams;
    for (int i = 0; i < hierarchy.size(); i++) {
        Declaration* param = hierarchy[i]->link;
        while (param && param->kind == Declaration::Parameter) {
            allParams.append(param);
            param = param->next;
        }
    }
    
    for (int i = 0; i < allParams.size(); i++) {
        if (i > 0) out << ", ";
        out << mapType(allParams[i]->type()) << " " << mangleVarName(allParams[i]);
    }
    if (allParams.isEmpty())
        out << "void";
    out << ") {\n";
    
    // Allocate object
    out << "    " << className << "* self = (" << className << "*)GC_MALLOC(sizeof(" << className << "));\n";
    out << "    self->_base._vt = (SimVtable*)&" << className << "_vtable;\n";
    
    // Initialize parameters
    for (int i = 0; i < allParams.size(); i++) {
        QString varName = mangleVarName(allParams[i]);
        out << "    self->" << varName << " = " << varName << ";\n";
    }
    
    // Initialize member variables to default values
    Declaration* member = cls->link;
    while (member) {
        if (member->kind == Declaration::Variable) {
            QString varName = mangleVarName(member);
            Type* t = member->type();
            if (t) {
                switch (t->kind) {
                case Type::Integer:
                case Type::ShortInteger:
                    out << "    self->" << varName << " = 0;\n";
                    break;
                case Type::Real:
                case Type::LongReal:
                    out << "    self->" << varName << " = 0.0;\n";
                    break;
                case Type::Boolean:
                    out << "    self->" << varName << " = false;\n";
                    break;
                case Type::Character:
                    out << "    self->" << varName << " = '\\0';\n";
                    break;
                case Type::Text:
                    out << "    self->" << varName << " = sim_notext();\n";
                    break;
                case Type::Ref:
                    out << "    self->" << varName << " = NULL;\n";
                    break;
                default:
                    break;
                }
            }
        } else if (member->kind == Declaration::Array) {
            QString varName = mangleVarName(member);
            out << "    self->" << varName << " = NULL; /* Array initialized later */\n";
        }
        member = member->next;
    }
    
    // Execute class body
    if (cls->body) {
        out << "    /* Class body */\n";
        out << "    {\n";
        indentLevel = 2;
        emitStatementSeq(cls->body, out);
        indentLevel = 0;
        out << "    }\n";
    }
    
    out << "    return self;\n";
    out << "}\n\n";
}

void CeeGen::collectClassHierarchy(Declaration* cls, QList<Declaration*>& hierarchy)
{
    if (!cls)
        return;
    
    if (cls->prefix)
        collectClassHierarchy(cls->prefix, hierarchy);
    
    hierarchy.append(cls);
}

Declaration* CeeGen::findVirtualMatch(Declaration* cls, Declaration* vspec)
{
    if (!cls || !vspec)
        return 0;
    
    // Search in current class
    Declaration* member = cls->link;
    while (member) {
        if (member->kind == Declaration::Procedure && member->sym == vspec->sym)
            return member;
        member = member->next;
    }
    
    // Search in prefix
    if (cls->prefix)
        return findVirtualMatch(cls->prefix, vspec);
    
    return 0;
}

bool CeeGen::isSubclassOf(Declaration* sub, Declaration* super)
{
    if (!sub || !super)
        return false;
    
    if (sub == super)
        return true;
    
    if (sub->prefix)
        return isSubclassOf(sub->prefix, super);
    
    return false;
}

int CeeGen::getClassId(Declaration* cls)
{
    if (!cls)
        return 0;
    
    if (classIdMap.contains(cls))
        return classIdMap[cls].toInt();
    
    static int nextId = 1;
    int id = nextId++;
    classIdMap[cls] = QString::number(id);
    return id;
}

// ============================================================================
// Procedure Generation
// ============================================================================

void CeeGen::emitProcedure(Declaration* proc)
{
    if (!proc || emittedProcs.contains(proc))
        return;
    
    if (isBuiltinProc(proc))
        return;
    
    emittedProcs.insert(proc);
    
    QTextStream out(&funcDefs);
    
    currentProc = proc;
    
    // Return type
    QString retType = proc->type() ? mapType(proc->type()) : "void";
    
    // Function name
    QString funcName = mangleProcName(proc);
    
    // Check if this is a class member
    bool isClassMember = proc->outer && 
        (proc->outer->kind == Declaration::Class || proc->outer->kind == Declaration::StandardClass);
    
    out << retType << " " << funcName << "(";
    
    // If class member, first parameter is self
    if (isClassMember) {
        out << mangleClassName(proc->outer) << "* self";
    }
    
    // Parameters
    Declaration* param = proc->link;
    bool first = !isClassMember;
    while (param && param->kind == Declaration::Parameter) {
        if (!first) out << ", ";
        first = false;
        
        QString paramType = mapType(param->type());
        QString paramName = mangleVarName(param);
        
        // Handle call by name
        if (param->mode == Declaration::ModeName) {
            out << "SimThunk* " << paramName << "_thunk";
        } else {
            out << paramType << " " << paramName;
        }
        
        param = param->next;
    }
    
    if (first)
        out << "void";
    
    out << ") {\n";
    
    // Local variables
    Declaration* local = proc->link;
    while (local) {
        if (local->kind == Declaration::Variable) {
            QString varType = mapType(local->type());
            QString varName = mangleVarName(local);
            out << "    " << varType << " " << varName;
            
            // Initialize
            Type* t = local->type();
            if (t) {
                switch (t->kind) {
                case Type::Integer:
                case Type::ShortInteger:
                    out << " = 0";
                    break;
                case Type::Real:
                case Type::LongReal:
                    out << " = 0.0";
                    break;
                case Type::Boolean:
                    out << " = false";
                    break;
                case Type::Character:
                    out << " = '\\0'";
                    break;
                case Type::Text:
                    out << " = sim_notext()";
                    break;
                case Type::Ref:
                    out << " = NULL";
                    break;
                default:
                    break;
                }
            }
            out << ";\n";
        } else if (local->kind == Declaration::Array) {
            QString varName = mangleVarName(local);
            out << "    SimArray* " << varName << " = NULL;\n";
        }
        local = local->next;
    }
    
    // Return value variable if typed procedure
    if (proc->type() && proc->type()->kind != Type::NoType) {
        out << "    " << retType << " _result";
        Type* t = proc->type();
        switch (t->kind) {
        case Type::Integer:
        case Type::ShortInteger:
            out << " = 0";
            break;
        case Type::Real:
        case Type::LongReal:
            out << " = 0.0";
            break;
        case Type::Boolean:
            out << " = false";
            break;
        case Type::Character:
            out << " = '\\0'";
            break;
        case Type::Text:
            out << " = sim_notext()";
            break;
        case Type::Ref:
            out << " = NULL";
            break;
        default:
            break;
        }
        out << ";\n";
    }
    
    out << "\n";
    
    // Procedure body
    if (proc->body) {
        indentLevel = 1;
        emitStatementSeq(proc->body, out);
        indentLevel = 0;
    }
    
    // Return statement
    if (proc->type() && proc->type()->kind != Type::NoType) {
        out << "    return _result;\n";
    }
    
    out << "}\n\n";
    
    currentProc = 0;
}

void CeeGen::emitVariable(Declaration* var)
{
    // Global variables are emitted in the module scope
    if (!var || var->outer->kind == Declaration::Module || var->outer->kind == Declaration::Program)
        return;
    
    // Local variables are handled in procedure/class emission
}

void CeeGen::emitArray(Declaration* arr)
{
    // Arrays are handled in procedure/class emission
}

void CeeGen::emitSwitch(Declaration* sw)
{
    // Switches are handled specially
}

void CeeGen::emitParameter(Declaration* param)
{
    // Parameters are handled in procedure emission
}

void CeeGen::emitBlock(Declaration* blk)
{
    // Blocks are handled as statements
}

// ============================================================================
// Statement Generation
// ============================================================================

void CeeGen::emitStatement(Statement* s, QTextStream& out)
{
    if (!s)
        return;
    
    switch (s->kind) {
    case Statement::Compound:
        emitCompound(s, out);
        break;
    case Statement::Block:
        emitBlockStmt(s, out);
        break;
    case Statement::Assign:
        emitAssign(s, out);
        break;
    case Statement::Call:
        emitCallStmt(s, out);
        break;
    case Statement::If:
        emitIf(s, out);
        break;
    case Statement::While:
        emitWhile(s, out);
        break;
    case Statement::For:
        emitFor(s, out);
        break;
    case Statement::Inspect:
        emitInspect(s, out);
        break;
    case Statement::Goto:
        emitGoto(s, out);
        break;
    case Statement::Label:
        emitLabel(s, out);
        break;
    case Statement::Inner:
        emitInner(s, out);
        break;
    case Statement::Activate:
        emitActivate(s, out);
        break;
    case Statement::Detach:
        emitDetach(s, out);
        break;
    case Statement::Resume:
        emitResume(s, out);
        break;
    case Statement::Dummy:
    case Statement::End:
        // No code needed
        break;
    default:
        out << indent() << "/* Unknown statement kind */\n";
        break;
    }
}

void CeeGen::emitStatementSeq(Statement* s, QTextStream& out)
{
    while (s) {
        emitStatement(s, out);
        s = s->next;
    }
}

void CeeGen::emitCompound(Statement* s, QTextStream& out)
{
    out << indent() << "{\n";
    increaseIndent();
    
    // Emit local declarations if any
    if (s->scope) {
        Declaration* local = s->scope->link;
        while (local) {
            if (local->kind == Declaration::Variable) {
                QString varType = mapType(local->type());
                QString varName = mangleVarName(local);
                out << indent() << varType << " " << varName << ";\n";
            }
            local = local->next;
        }
    }
    
    if (s->body)
        emitStatementSeq(s->body, out);
    
    decreaseIndent();
    out << indent() << "}\n";
}

void CeeGen::emitBlockStmt(Statement* s, QTextStream& out)
{
    out << indent() << "{\n";
    increaseIndent();
    
    // Emit local declarations
    if (s->scope) {
        Declaration* local = s->scope->link;
        while (local) {
            if (local->kind == Declaration::Variable) {
                QString varType = mapType(local->type());
                QString varName = mangleVarName(local);
                out << indent() << varType << " " << varName;
                
                // Initialize
                Type* t = local->type();
                if (t) {
                    switch (t->kind) {
                    case Type::Integer:
                    case Type::ShortInteger:
                        out << " = 0";
                        break;
                    case Type::Real:
                    case Type::LongReal:
                        out << " = 0.0";
                        break;
                    case Type::Boolean:
                        out << " = false";
                        break;
                    case Type::Character:
                        out << " = '\\0'";
                        break;
                    case Type::Text:
                        out << " = sim_notext()";
                        break;
                    case Type::Ref:
                        out << " = NULL";
                        break;
                    default:
                        break;
                    }
                }
                out << ";\n";
            } else if (local->kind == Declaration::Array) {
                QString varName = mangleVarName(local);
                out << indent() << "SimArray* " << varName << " = NULL;\n";
            }
            local = local->next;
        }
    }
    
    // Handle prefixed block (class instantiation)
    if (s->prefix) {
        QString prefixExpr = emitExpr(s->prefix, out);
        // TODO: Handle prefixed blocks properly
        out << indent() << "/* Prefixed block: " << prefixExpr << " */\n";
    }
    
    if (s->body)
        emitStatementSeq(s->body, out);
    
    decreaseIndent();
    out << indent() << "}\n";
}

void CeeGen::emitAssign(Statement* s, QTextStream& out)
{
    if (!s->lhs || !s->rhs)
        return;
    
    QString lhsExpr = emitExpr(s->lhs, out);
    QString rhsExpr = emitExpr(s->rhs, out);
    
    out << indent() << lhsExpr << " = " << rhsExpr << ";\n";
}

void CeeGen::emitCallStmt(Statement* s, QTextStream& out)
{
    if (!s->lhs)
        return;
    
    QString callExpr = emitExpr(s->lhs, out);
    out << indent() << callExpr << ";\n";
}

void CeeGen::emitIf(Statement* s, QTextStream& out)
{
    QString condExpr = emitExpr(s->cond, out);
    
    out << indent() << "if (" << condExpr << ") {\n";
    increaseIndent();
    
    if (s->body)
        emitStatementSeq(s->body, out);
    
    decreaseIndent();
    out << indent() << "}";
    
    if (s->elseStmt) {
        out << " else {\n";
        increaseIndent();
        emitStatementSeq(s->elseStmt, out);
        decreaseIndent();
        out << indent() << "}";
    }
    
    out << "\n";
}

void CeeGen::emitWhile(Statement* s, QTextStream& out)
{
    QString condExpr = emitExpr(s->cond, out);
    
    out << indent() << "while (" << condExpr << ") {\n";
    increaseIndent();
    
    if (s->body)
        emitStatementSeq(s->body, out);
    
    decreaseIndent();
    out << indent() << "}\n";
}

void CeeGen::emitFor(Statement* s, QTextStream& out)
{
    if (!s->var || !s->list)
        return;
    
    QString varExpr = emitExpr(s->var, out);
    
    // Handle each for-list element
    Expression* elem = s->list;
    while (elem) {
        if (elem->kind == Expression::StepUntil) {
            // for i := start step step until limit do
            QString startExpr = emitExpr(elem->lhs, out);
            QString stepExpr = emitExpr(elem->rhs, out);
            QString limitExpr = emitExpr(elem->condition, out);
            
            out << indent() << "for (" << varExpr << " = " << startExpr << "; ";
            out << "(" << stepExpr << " >= 0 ? " << varExpr << " <= " << limitExpr;
            out << " : " << varExpr << " >= " << limitExpr << "); ";
            out << varExpr << " += " << stepExpr << ") {\n";
            
            increaseIndent();
            if (s->body)
                emitStatementSeq(s->body, out);
            decreaseIndent();
            
            out << indent() << "}\n";
        } else if (elem->kind == Expression::WhileLoop) {
            // for i := expr while cond do
            QString startExpr = emitExpr(elem->lhs, out);
            QString condExpr = emitExpr(elem->condition, out);
            
            out << indent() << varExpr << " = " << startExpr << ";\n";
            out << indent() << "while (" << condExpr << ") {\n";
            
            increaseIndent();
            if (s->body)
                emitStatementSeq(s->body, out);
            out << indent() << varExpr << " = " << startExpr << ";\n";
            decreaseIndent();
            
            out << indent() << "}\n";
        } else {
            // Simple expression
            QString valExpr = emitExpr(elem, out);
            out << indent() << varExpr << " = " << valExpr << ";\n";
            out << indent() << "{\n";
            increaseIndent();
            if (s->body)
                emitStatementSeq(s->body, out);
            decreaseIndent();
            out << indent() << "}\n";
        }
        
        elem = elem->next;
    }
}

void CeeGen::emitInspect(Statement* s, QTextStream& out)
{
    if (!s->obj)
        return;
    
    QString objExpr = emitExpr(s->obj, out);
    QString tempVar = newTempVar("inspect");
    
    out << indent() << "{\n";
    increaseIndent();
    out << indent() << "SimObject* " << tempVar << " = (SimObject*)" << objExpr << ";\n";
    
    // Emit WHEN clauses
    Connection* conn = s->conn;
    bool first = true;
    while (conn) {
        if (conn->classDecl) {
            QString className = mangleClassName(conn->classDecl);
            int classId = getClassId(conn->classDecl);
            
            out << indent();
            if (!first) out << "} else ";
            out << "if (" << tempVar << " != NULL && sim_is_instance(" << tempVar << ", " << classId << ")) {\n";
            
            increaseIndent();
            out << indent() << className << "* this_" << conn->className << " = (" << className << "*)" << tempVar << ";\n";
            
            if (conn->body)
                emitStatementSeq(conn->body, out);
            
            decreaseIndent();
            first = false;
        }
        conn = conn->next;
    }
    
    // OTHERWISE clause
    if (s->otherwise) {
        out << indent();
        if (!first) out << "} else ";
        out << "{\n";
        increaseIndent();
        emitStatementSeq(s->otherwise, out);
        decreaseIndent();
    }
    
    if (!first)
        out << indent() << "}\n";
    
    // DO clause
    if (s->body) {
        out << indent() << "/* DO clause */\n";
        emitStatementSeq(s->body, out);
    }
    
    decreaseIndent();
    out << indent() << "}\n";
}

void CeeGen::emitGoto(Statement* s, QTextStream& out)
{
    if (!s->lhs)
        return;
    
    // Simple label reference
    if (s->lhs->kind == Expression::DeclRef && s->lhs->d) {
        out << indent() << "goto " << mangleVarName(s->lhs->d) << ";\n";
    } else {
        // Computed goto (switch designator)
        QString labelExpr = emitExpr(s->lhs, out);
        out << indent() << "sim_goto(" << labelExpr << ");\n";
    }
}

void CeeGen::emitLabel(Statement* s, QTextStream& out)
{
    if (!s->label)
        return;
    
    out << mangleVarName(s->label) << ":;\n";
}

void CeeGen::emitInner(Statement* s, QTextStream& out)
{
    out << indent() << "/* INNER - subclass body would be called here */\n";
    // In the actual implementation, this would call the subclass body function
    // For now, we emit a placeholder
}

void CeeGen::emitActivate(Statement* s, QTextStream& out)
{
    out << indent() << "/* ACTIVATE - Stage 2 feature */\n";
    // Stage 2 implementation
}

void CeeGen::emitDetach(Statement* s, QTextStream& out)
{
    out << indent() << "sim_detach();\n";
}

void CeeGen::emitResume(Statement* s, QTextStream& out)
{
    if (s->lhs) {
        QString objExpr = emitExpr(s->lhs, out);
        out << indent() << "sim_resume(" << objExpr << ");\n";
    }
}

// ============================================================================
// Expression Generation
// ============================================================================

QString CeeGen::emitExpr(Expression* e, QTextStream& out)
{
    if (!e)
        return "NULL";
    
    switch (e->kind) {
    case Expression::Add:
    case Expression::Sub:
    case Expression::Mul:
    case Expression::Div:
    case Expression::IntDiv:
    case Expression::Exp:
    case Expression::And:
    case Expression::Or:
    case Expression::Imp:
    case Expression::Eqv:
    case Expression::AndThen:
    case Expression::OrElse:
    case Expression::Eq:
    case Expression::Neq:
    case Expression::Lt:
    case Expression::Leq:
    case Expression::Gt:
    case Expression::Geq:
    case Expression::RefEq:
    case Expression::RefNeq:
    case Expression::Is:
    case Expression::In:
        return emitBinaryOp(e, out);
        
    case Expression::Neg:
    case Expression::Not:
        return emitUnaryOp(e, out);
        
    case Expression::Identifier:
        return emitIdentifier(e, out);
        
    case Expression::DeclRef:
        return emitDeclRef(e, out);
        
    case Expression::Dot:
        return emitDot(e, out);
        
    case Expression::Subscript:
        return emitSubscript(e, out);
        
    case Expression::Call:
        return emitCall(e, out);
        
    case Expression::New:
        return emitNew(e, out);
        
    case Expression::This:
        return emitThis(e, out);
        
    case Expression::Qua:
        return emitQua(e, out);
        
    case Expression::IfExpr:
        return emitIfExpr(e, out);
        
    case Expression::StringConst:
    case Expression::CharConst:
    case Expression::UnsignedConst:
    case Expression::RealConst:
    case Expression::BoolConst:
    case Expression::Notext:
    case Expression::None:
        return emitLiteral(e, out);
        
    case Expression::AssignVal:
    case Expression::AssignRef:
        return emitAssignExpr(e, out);
        
    default:
        return "/* unknown expr */";
    }
}

QString CeeGen::emitBinaryOp(Expression* e, QTextStream& out)
{
    QString lhs = emitExpr(e->lhs, out);
    QString rhs = emitExpr(e->rhs, out);
    
    switch (e->kind) {
    case Expression::Add:
        return QString("(%1 + %2)").arg(lhs).arg(rhs);
    case Expression::Sub:
        return QString("(%1 - %2)").arg(lhs).arg(rhs);
    case Expression::Mul:
        return QString("(%1 * %2)").arg(lhs).arg(rhs);
    case Expression::Div:
        return QString("(%1 / %2)").arg(lhs).arg(rhs);
    case Expression::IntDiv:
        return QString("((%1) / (%2))").arg(lhs).arg(rhs);
    case Expression::Exp:
        return QString("pow(%1, %2)").arg(lhs).arg(rhs);
    case Expression::And:
        return QString("(%1 && %2)").arg(lhs).arg(rhs);
    case Expression::Or:
        return QString("(%1 || %2)").arg(lhs).arg(rhs);
    case Expression::AndThen:
        return QString("(%1 && %2)").arg(lhs).arg(rhs);
    case Expression::OrElse:
        return QString("(%1 || %2)").arg(lhs).arg(rhs);
    case Expression::Imp:
        return QString("(!(%1) || (%2))").arg(lhs).arg(rhs);
    case Expression::Eqv:
        return QString("((%1) == (%2))").arg(lhs).arg(rhs);
    case Expression::Eq:
        // Check if text comparison
        if (e->lhs && e->lhs->type() && e->lhs->type()->kind == Type::Text)
            return QString("sim_text_eq(%1, %2)").arg(lhs).arg(rhs);
        return QString("(%1 == %2)").arg(lhs).arg(rhs);
    case Expression::Neq:
        if (e->lhs && e->lhs->type() && e->lhs->type()->kind == Type::Text)
            return QString("!sim_text_eq(%1, %2)").arg(lhs).arg(rhs);
        return QString("(%1 != %2)").arg(lhs).arg(rhs);
    case Expression::Lt:
        if (e->lhs && e->lhs->type() && e->lhs->type()->kind == Type::Text)
            return QString("(sim_text_cmp(%1, %2) < 0)").arg(lhs).arg(rhs);
        return QString("(%1 < %2)").arg(lhs).arg(rhs);
    case Expression::Leq:
        if (e->lhs && e->lhs->type() && e->lhs->type()->kind == Type::Text)
            return QString("(sim_text_cmp(%1, %2) <= 0)").arg(lhs).arg(rhs);
        return QString("(%1 <= %2)").arg(lhs).arg(rhs);
    case Expression::Gt:
        if (e->lhs && e->lhs->type() && e->lhs->type()->kind == Type::Text)
            return QString("(sim_text_cmp(%1, %2) > 0)").arg(lhs).arg(rhs);
        return QString("(%1 > %2)").arg(lhs).arg(rhs);
    case Expression::Geq:
        if (e->lhs && e->lhs->type() && e->lhs->type()->kind == Type::Text)
            return QString("(sim_text_cmp(%1, %2) >= 0)").arg(lhs).arg(rhs);
        return QString("(%1 >= %2)").arg(lhs).arg(rhs);
    case Expression::RefEq:
        return QString("(%1 == %2)").arg(lhs).arg(rhs);
    case Expression::RefNeq:
        return QString("(%1 != %2)").arg(lhs).arg(rhs);
    case Expression::Is:
        if (e->rhs && e->rhs->kind == Expression::DeclRef && e->rhs->d) {
            int classId = getClassId(e->rhs->d);
            return QString("sim_is_exact((SimObject*)%1, %2)").arg(lhs).arg(classId);
        }
        return QString("sim_is_exact((SimObject*)%1, 0)").arg(lhs);
    case Expression::In:
        if (e->rhs && e->rhs->kind == Expression::DeclRef && e->rhs->d) {
            int classId = getClassId(e->rhs->d);
            return QString("sim_is_instance((SimObject*)%1, %2)").arg(lhs).arg(classId);
        }
        return QString("sim_is_instance((SimObject*)%1, 0)").arg(lhs);
    default:
        return QString("(%1 ? %2)").arg(lhs).arg(rhs);
    }
}

QString CeeGen::emitUnaryOp(Expression* e, QTextStream& out)
{
    QString operand = emitExpr(e->lhs, out);
    
    switch (e->kind) {
    case Expression::Neg:
        return QString("(-%1)").arg(operand);
    case Expression::Not:
        return QString("(!%1)").arg(operand);
    default:
        return operand;
    }
}

QString CeeGen::emitIdentifier(Expression* e, QTextStream& out)
{
    if (e->a)
        return QString(e->a);
    return "/* unknown identifier */";
}

QString CeeGen::emitDeclRef(Expression* e, QTextStream& out)
{
    if (!e->d)
        return "NULL";
    
    Declaration* d = e->d;
    
    // Check if it's a class member access
    if (currentClass && d->outer == currentClass) {
        return QString("self->%1").arg(mangleVarName(d));
    }
    
    // Check if it's a procedure parameter
    if (currentProc && d->outer == currentProc && d->kind == Declaration::Parameter) {
        if (d->mode == Declaration::ModeName) {
            return QString("sim_thunk_eval(%1_thunk)").arg(mangleVarName(d));
        }
        return mangleVarName(d);
    }
    
    // Check if it's a local variable
    if (d->kind == Declaration::Variable || d->kind == Declaration::Parameter) {
        return mangleVarName(d);
    }
    
    // Check if it's a procedure
    if (d->kind == Declaration::Procedure) {
        if (isBuiltinProc(d))
            return getBuiltinProcName(d);
        return mangleProcName(d);
    }
    
    // Check if it's a class
    if (d->kind == Declaration::Class || d->kind == Declaration::StandardClass) {
        return mangleClassName(d);
    }
    
    return mangleVarName(d);
}

QString CeeGen::emitDot(Expression* e, QTextStream& out)
{
    QString lhs = emitExpr(e->lhs, out);
    
    if (e->rhs && e->rhs->kind == Expression::DeclRef && e->rhs->d) {
        Declaration* member = e->rhs->d;
        
        // Check if it's a procedure call
        if (member->kind == Declaration::Procedure) {
            // This will be handled by emitCall
            return QString("%1->%2").arg(lhs).arg(mangleVarName(member));
        }
        
        return QString("%1->%2").arg(lhs).arg(mangleVarName(member));
    }
    
    if (e->rhs && e->rhs->kind == Expression::Identifier && e->rhs->a) {
        return QString("%1->%2").arg(lhs).arg(e->rhs->a);
    }
    
    return lhs;
}

QString CeeGen::emitSubscript(Expression* e, QTextStream& out)
{
    QString arr = emitExpr(e->lhs, out);
    
    // Collect indices
    QStringList indices;
    Expression* idx = e->rhs;
    while (idx) {
        indices.append(emitExpr(idx, out));
        idx = idx->next;
    }
    
    if (indices.size() == 1) {
        return QString("sim_array_get(%1, %2)").arg(arr).arg(indices[0]);
    } else {
        QString idxList = indices.join(", ");
        return QString("sim_array_get_multi(%1, %2, %3)").arg(arr).arg(indices.size()).arg(idxList);
    }
}

QString CeeGen::emitCall(Expression* e, QTextStream& out)
{
    if (!e->lhs)
        return "/* invalid call */";
    
    // Check if it's a dot call (method call)
    if (e->lhs->kind == Expression::Dot) {
        QString obj = emitExpr(e->lhs->lhs, out);
        
        if (e->lhs->rhs && e->lhs->rhs->kind == Expression::DeclRef && e->lhs->rhs->d) {
            Declaration* proc = e->lhs->rhs->d;
            
            // Check for TEXT methods
            if (e->lhs->lhs && e->lhs->lhs->type() && e->lhs->lhs->type()->kind == Type::Text) {
                QString methodName = QString(proc->name.constData()).toLower();
                
                if (methodName == "length") return QString("sim_text_length(%1)").arg(obj);
                if (methodName == "pos") return QString("sim_text_pos(%1)").arg(obj);
                if (methodName == "setpos") {
                    QString arg = e->rhs ? emitExpr(e->rhs, out) : "1";
                    return QString("sim_text_setpos(&%1, %2)").arg(obj).arg(arg);
                }
                if (methodName == "more") return QString("sim_text_more(%1)").arg(obj);
                if (methodName == "getchar") return QString("sim_text_getchar(&%1)").arg(obj);
                if (methodName == "putchar") {
                    QString arg = e->rhs ? emitExpr(e->rhs, out) : "'\\0'";
                    return QString("sim_text_putchar(&%1, %2)").arg(obj).arg(arg);
                }
                if (methodName == "sub") {
                    QString arg1 = "1", arg2 = "0";
                    if (e->rhs) {
                        arg1 = emitExpr(e->rhs, out);
                        if (e->rhs->next)
                            arg2 = emitExpr(e->rhs->next, out);
                    }
                    return QString("sim_text_sub(%1, %2, %3)").arg(obj).arg(arg1).arg(arg2);
                }
                if (methodName == "strip") return QString("sim_text_strip(%1)").arg(obj);
                if (methodName == "getint") return QString("sim_text_getint(&%1)").arg(obj);
                if (methodName == "getreal") return QString("sim_text_getreal(&%1)").arg(obj);
                if (methodName == "putint") {
                    QString arg1 = "0", arg2 = "0";
                    if (e->rhs) {
                        arg1 = emitExpr(e->rhs, out);
                        if (e->rhs->next)
                            arg2 = emitExpr(e->rhs->next, out);
                    }
                    return QString("sim_text_putint(&%1, %2, %3)").arg(obj).arg(arg1).arg(arg2);
                }
                if (methodName == "putfix") {
                    QString arg1 = "0.0", arg2 = "0", arg3 = "0";
                    if (e->rhs) {
                        arg1 = emitExpr(e->rhs, out);
                        if (e->rhs->next) {
                            arg2 = emitExpr(e->rhs->next, out);
                            if (e->rhs->next->next)
                                arg3 = emitExpr(e->rhs->next->next, out);
                        }
                    }
                    return QString("sim_text_putfix(&%1, %2, %3, %4)").arg(obj).arg(arg1).arg(arg2).arg(arg3);
                }
                if (methodName == "putreal") {
                    QString arg1 = "0.0", arg2 = "0", arg3 = "0";
                    if (e->rhs) {
                        arg1 = emitExpr(e->rhs, out);
                        if (e->rhs->next) {
                            arg2 = emitExpr(e->rhs->next, out);
                            if (e->rhs->next->next)
                                arg3 = emitExpr(e->rhs->next->next, out);
                        }
                    }
                    return QString("sim_text_putreal(&%1, %2, %3, %4)").arg(obj).arg(arg1).arg(arg2).arg(arg3);
                }
            }
            
            // Regular method call
            if (isBuiltinProc(proc)) {
                QString funcName = getBuiltinProcName(proc);
                QStringList args;
                args.append(obj);
                Expression* arg = e->rhs;
                while (arg) {
                    args.append(emitExpr(arg, out));
                    arg = arg->next;
                }
                return QString("%1(%2)").arg(funcName).arg(args.join(", "));
            }
            
            QString funcName = mangleProcName(proc);
            QStringList args;
            args.append(obj);
            Expression* arg = e->rhs;
            while (arg) {
                args.append(emitExpr(arg, out));
                arg = arg->next;
            }
            return QString("%1(%2)").arg(funcName).arg(args.join(", "));
        }
    }
    
    // Direct procedure call
    if (e->lhs->kind == Expression::DeclRef && e->lhs->d) {
        Declaration* proc = e->lhs->d;
        
        QString funcName;
        if (isBuiltinProc(proc))
            funcName = getBuiltinProcName(proc);
        else
            funcName = mangleProcName(proc);
        
        QStringList args;
        
        // If class member procedure, add self
        if (currentClass && proc->outer == currentClass) {
            args.append("self");
        }
        
        Expression* arg = e->rhs;
        while (arg) {
            args.append(emitExpr(arg, out));
            arg = arg->next;
        }
        
        if (args.isEmpty())
            return QString("%1()").arg(funcName);
        return QString("%1(%2)").arg(funcName).arg(args.join(", "));
    }
    
    // Fallback
    QString callable = emitExpr(e->lhs, out);
    QStringList args;
    Expression* arg = e->rhs;
    while (arg) {
        args.append(emitExpr(arg, out));
        arg = arg->next;
    }
    
    if (args.isEmpty())
        return QString("%1()").arg(callable);
    return QString("%1(%2)").arg(callable).arg(args.join(", "));
}

QString CeeGen::emitNew(Expression* e, QTextStream& out)
{
    if (!e->lhs)
        return "NULL";
    
    Declaration* cls = 0;
    if (e->lhs->kind == Expression::DeclRef)
        cls = e->lhs->d;
    else if (e->lhs->kind == Expression::Identifier) {
        // Need to resolve
        return QString("/* NEW %1 - unresolved */").arg(e->lhs->a ? e->lhs->a : "?");
    }
    
    if (!cls)
        return "NULL";
    
    QString className = mangleClassName(cls);
    
    // Collect arguments
    QStringList args;
    Expression* arg = e->rhs;
    while (arg) {
        args.append(emitExpr(arg, out));
        arg = arg->next;
    }
    
    if (args.isEmpty())
        return QString("%1_new()").arg(className);
    return QString("%1_new(%2)").arg(className).arg(args.join(", "));
}

QString CeeGen::emitThis(Expression* e, QTextStream& out)
{
    // THIS class-identifier
    if (e->lhs && e->lhs->kind == Expression::DeclRef && e->lhs->d) {
        QString className = mangleClassName(e->lhs->d);
        return QString("((%1*)self)").arg(className);
    }
    
    return "self";
}

QString CeeGen::emitQua(Expression* e, QTextStream& out)
{
    QString obj = emitExpr(e->lhs, out);
    
    if (e->rhs && e->rhs->kind == Expression::DeclRef && e->rhs->d) {
        QString className = mangleClassName(e->rhs->d);
        int classId = getClassId(e->rhs->d);
        return QString("sim_qua(%1, %2, \"%3\")").arg(obj).arg(classId).arg(className);
    }
    
    return obj;
}

QString CeeGen::emitIfExpr(Expression* e, QTextStream& out)
{
    QString cond = emitExpr(e->condition, out);
    QString thenExpr = emitExpr(e->lhs, out);
    QString elseExpr = emitExpr(e->rhs, out);
    
    return QString("((%1) ? (%2) : (%3))").arg(cond).arg(thenExpr).arg(elseExpr);
}

QString CeeGen::emitLiteral(Expression* e, QTextStream& out)
{
    switch (e->kind) {
    case Expression::UnsignedConst:
        return QString::number(e->u);
        
    case Expression::RealConst:
        return QString::number(e->r, 'g', 17);
        
    case Expression::BoolConst:
        return e->u ? "true" : "false";
        
    case Expression::CharConst:
        if (e->u == '\'')
            return "'\\''";
        if (e->u == '\\')
            return "'\\\\'";
        if (e->u == '\n')
            return "'\\n'";
        if (e->u == '\t')
            return "'\\t'";
        if (e->u == '\r')
            return "'\\r'";
        if (e->u >= 32 && e->u < 127)
            return QString("'%1'").arg(QChar((char)e->u));
        return QString("'\\x%1'").arg(e->u, 2, 16, QChar('0'));
        
    case Expression::StringConst:
        if (e->a) {
            QString str = QString::fromUtf8(e->a);
            str.replace("\\", "\\\\");
            str.replace("\"", "\\\"");
            str.replace("\n", "\\n");
            str.replace("\t", "\\t");
            str.replace("\r", "\\r");
            return QString("sim_text_const(\"%1\")").arg(str);
        }
        return "sim_notext()";
        
    case Expression::Notext:
        return "sim_notext()";
        
    case Expression::None:
        return "NULL";
        
    default:
        return "0";
    }
}

QString CeeGen::emitAssignExpr(Expression* e, QTextStream& out)
{
    QString lhs = emitExpr(e->lhs, out);
    QString rhs = emitExpr(e->rhs, out);
    
    if (e->kind == Expression::AssignRef) {
        // Reference assignment
        return QString("(%1 = %2)").arg(lhs).arg(rhs);
    }
    
    // Value assignment
    // Check for TEXT assignment
    if (e->lhs && e->lhs->type() && e->lhs->type()->kind == Type::Text) {
        return QString("sim_text_assign(&%1, %2)").arg(lhs).arg(rhs);
    }
    
    return QString("(%1 = %2)").arg(lhs).arg(rhs);
}

QString CeeGen::emitBuiltinCall(Declaration* proc, Expression* args, QTextStream& out)
{
    QString funcName = getBuiltinProcName(proc);
    
    QStringList argList;
    Expression* arg = args;
    while (arg) {
        argList.append(emitExpr(arg, out));
        arg = arg->next;
    }
    
    if (argList.isEmpty())
        return QString("%1()").arg(funcName);
    return QString("%1(%2)").arg(funcName).arg(argList.join(", "));
}
