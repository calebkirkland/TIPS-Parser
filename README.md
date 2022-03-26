# TIPS-Parser
Parser for the TIPS (Ten Instruction Pascal Subeset) programming languag.

To compile, go into the directory of the project and type:
`make` and then 
`./tips_parser input1.pas`
etc.
To test the parse outputs against the test, you can expect no output from the diff command:
`./tips_parse input1.pas | diff - input1.corret`

