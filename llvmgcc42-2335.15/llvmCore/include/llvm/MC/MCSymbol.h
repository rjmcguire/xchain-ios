//===- MCSymbol.h - Machine Code Symbols ------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the MCSymbol class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_MC_MCSYMBOL_H
#define LLVM_MC_MCSYMBOL_H

#include "llvm/ADT/StringRef.h"

namespace llvm {
  class MCExpr;
  class MCSection;
  class MCContext;
  class raw_ostream;

  /// MCSymbol - Instances of this class represent a symbol name in the MC file,
  /// and MCSymbols are created and unique'd by the MCContext class.  MCSymbols
  /// should only be constructed with valid names for the object file.
  ///
  /// If the symbol is defined/emitted into the current translation unit, the
  /// Section member is set to indicate what section it lives in.  Otherwise, if
  /// it is a reference to an external entity, it has a null section.  
  /// 
  class MCSymbol {
    // Special sentinal value for the absolute pseudo section.
    //
    // FIXME: Use a PointerInt wrapper for this?
    static const MCSection *AbsolutePseudoSection;

    /// Name - The name of the symbol.  The referred-to string data is actually
    /// held by the StringMap that lives in MCContext.
    StringRef Name;

    /// Section - The section the symbol is defined in. This is null for
    /// undefined symbols, and the special AbsolutePseudoSection value for
    /// absolute symbols.
    const MCSection *Section;

    /// Value - If non-null, the value for a variable symbol.
    const MCExpr *Value;

    /// IsTemporary - True if this is an assembler temporary label, which
    /// typically does not survive in the .o file's symbol table.  Usually
    /// "Lfoo" or ".foo".
    unsigned IsTemporary : 1;
    
  private:  // MCContext creates and uniques these.
    friend class MCContext;
    MCSymbol(StringRef name, bool isTemporary)
      : Name(name), Section(0), Value(0), IsTemporary(isTemporary) {}

    MCSymbol(const MCSymbol&);       // DO NOT IMPLEMENT
    void operator=(const MCSymbol&); // DO NOT IMPLEMENT
  public:
    /// getName - Get the symbol name.
    StringRef getName() const { return Name; }

    /// @name Symbol Type
    /// @{

    /// isTemporary - Check if this is an assembler temporary symbol.
    bool isTemporary() const {
      return IsTemporary;
    }

    /// @}
    /// @name Associated Sections
    /// @{

    /// isDefined - Check if this symbol is defined (i.e., it has an address).
    ///
    /// Defined symbols are either absolute or in some section.
    bool isDefined() const {
      return Section != 0;
    }

    /// isUndefined - Check if this symbol undefined (i.e., implicitly defined).
    bool isUndefined() const {
      return !isDefined();
    }

    /// isAbsolute - Check if this is an absolute symbol.
    bool isAbsolute() const {
      return Section == AbsolutePseudoSection;
    }

    /// getSection - Get the section associated with a defined, non-absolute
    /// symbol.
    const MCSection &getSection() const {
      assert(!isUndefined() && !isAbsolute() && "Invalid accessor!");
      return *Section;
    }

    /// setSection - Mark the symbol as defined in the section \arg S.
    void setSection(const MCSection &S) { Section = &S; }

    /// setUndefined - Mark the symbol as undefined.
    void setUndefined() {
      Section = 0;
    }

    /// setAbsolute - Mark the symbol as absolute.
    void setAbsolute() { Section = AbsolutePseudoSection; }

    /// @}
    /// @name Variable Symbols
    /// @{

    /// isVariable - Check if this is a variable symbol.
    bool isVariable() const {
      return Value != 0;
    }

    /// getValue() - Get the value for variable symbols, or null if the symbol
    /// is not a variable.
    const MCExpr *getValue() const { return Value; }

    void setValue(const MCExpr *Value) {
      this->Value = Value;
    }

    /// @}

    /// print - Print the value to the stream \arg OS.
    void print(raw_ostream &OS) const;

    /// dump - Print the value to stderr.
    void dump() const;
  };

  inline raw_ostream &operator<<(raw_ostream &OS, const MCSymbol &Sym) {
    Sym.print(OS);
    return OS;
  }
} // end namespace llvm

#endif
