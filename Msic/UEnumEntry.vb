Option Strict Off
Option Explicit Off
Imports System
Imports EnvDTE
Imports EnvDTE80
Imports EnvDTE90
Imports EnvDTE90a
Imports EnvDTE100
Imports System.Diagnostics

Public Module UEnumEntry


    Sub UEnumReplace()
        DTE.Find.FindWhat = "enum[:b]+{:i}([^\n]|[\n])#\{{([\n]|[^\n])#}\};"
        DTE.Find.ReplaceWith = "UENUM_BEGIN(\1)\2UENUM_END"
        DTE.Find.Target = vsFindTarget.vsFindTargetCurrentDocumentSelection
        DTE.Find.MatchCase = False
        DTE.Find.MatchWholeWord = False
        DTE.Find.MatchInHiddenText = False
        DTE.Find.PatternSyntax = vsFindPatternSyntax.vsFindPatternSyntaxRegExpr
        DTE.Find.ResultsLocation = vsFindResultsLocation.vsFindResultsNone
        DTE.Find.Action = vsFindAction.vsFindActionReplaceAll
        If (DTE.Find.Execute() = vsFindResult.vsFindResultNotFound) Then
            'Throw New System.Exception("vsFindResultNotFound")
        End If

        DTE.Windows.Item("{CF2DDC32-8CAD-11D2-9302-005345000000}").Activate() '查找和替换
        DTE.Find.FindWhat = "^:b+{:i}([^\n]|[\n])#,"
        DTE.Find.ReplaceWith = "  UENUM_ENTRY(\1)"
        DTE.Find.Target = vsFindTarget.vsFindTargetCurrentDocumentSelection
        DTE.Find.MatchCase = False
        DTE.Find.MatchWholeWord = False
        DTE.Find.MatchInHiddenText = False
        DTE.Find.PatternSyntax = vsFindPatternSyntax.vsFindPatternSyntaxRegExpr
        DTE.Find.ResultsLocation = vsFindResultsLocation.vsFindResultsNone
        DTE.Find.Action = vsFindAction.vsFindActionReplaceAll
        If (DTE.Find.Execute() = vsFindResult.vsFindResultNotFound) Then
            'Throw New System.Exception("vsFindResultNotFound")
        End If
    End Sub


End Module
