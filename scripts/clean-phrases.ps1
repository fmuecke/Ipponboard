# Clean QPH files by removing duplicates and filtering out long or punctuation-containing phrases.

[CmdletBinding()]
param(
  [string]$InputFile = "i18n/Ipponboard_phrases_de.qph",
  [string]$OutputFile,
  [int]$MaxLength = 40
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

if (-not (Test-Path $InputFile)) {
  throw "Input QPH not found: $InputFile"
}

if (-not $OutputFile) {
  $OutputFile = $InputFile
}

[xml]$doc = Get-Content -LiteralPath $InputFile -Encoding UTF8 -Raw
$qphRoot = $doc.DocumentElement
if (-not $qphRoot) {
  throw "Invalid QPH structure (missing <QPH> root)."
}

$languageAttr = $qphRoot.GetAttribute("language")
$sourcelanguageAttr = $qphRoot.GetAttribute("sourcelanguage")

$phrases = [System.Collections.Generic.Dictionary[string, string]]::new()

foreach ($phrase in $qphRoot.SelectNodes("phrase")) {
  $source = [string]$phrase.source
  $target = [string]$phrase.target

  if ([string]::IsNullOrWhiteSpace($source) -or [string]::IsNullOrWhiteSpace($target)) { continue }

  $normalized = $source.Trim()
  if ($normalized.Length -gt $MaxLength) { continue }
  if ($normalized -match "[\.\?!]") { continue }

  if (-not $phrases.ContainsKey($normalized)) {
    $phrases[$normalized] = $target.Trim()
  }
}

$settings = [System.Xml.XmlWriterSettings]::new()
$settings.OmitXmlDeclaration = $true
$settings.Indent = $true
$settings.IndentChars = "  "

$stringWriter = [System.IO.StringWriter]::new()
$writer = [System.Xml.XmlWriter]::Create($stringWriter, $settings)
$writer.WriteStartElement("QPH")
if ($languageAttr) { $writer.WriteAttributeString("language", $languageAttr) }
if ($sourcelanguageAttr) { $writer.WriteAttributeString("sourcelanguage", $sourcelanguageAttr) }

foreach ($key in ($phrases.Keys | Sort-Object)) {
  $writer.WriteStartElement("phrase")
  $writer.WriteElementString("source", $key)
  $writer.WriteElementString("target", $phrases[$key])
  $writer.WriteEndElement()
}

$writer.WriteEndElement()
$writer.Flush()
$writer.Close()

$content = "<!DOCTYPE QPH>`n" + $stringWriter.ToString()
Set-Content -LiteralPath $OutputFile -Encoding UTF8 -Value $content
Write-Host "Wrote $OutputFile with $($phrases.Count) unique phrases."
