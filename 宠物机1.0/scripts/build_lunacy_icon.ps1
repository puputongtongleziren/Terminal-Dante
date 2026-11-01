param(
  [Parameter(Mandatory=$true)][string]$InputPng,
  [string]$Output = ''
)
$ErrorActionPreference = 'Stop'
if ([string]::IsNullOrWhiteSpace($Output)) { $Output = Join-Path $PSScriptRoot '..\src\lunacy_icon.h' }
Add-Type -AssemblyName System.Drawing
$src = [Drawing.Bitmap]::new($InputPng)
$dst = [Drawing.Bitmap]::new(20, 18)
$g = [Drawing.Graphics]::FromImage($dst)
$g.Clear([Drawing.Color]::Transparent)
$g.InterpolationMode = [Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
$g.DrawImage($src, 0, 0, 20, 18)
$g.Dispose(); $src.Dispose()
$writer = [IO.StreamWriter]::new([IO.Path]::GetFullPath($Output), $false, [Text.UTF8Encoding]::new($false))
try {
  $writer.WriteLine('// Generated Lunacy icon, RGB565; transparent pixels become black.')
  $writer.WriteLine('#pragma once')
  $writer.WriteLine('static const uint16_t PROGMEM lunacy_icon[360] = {')
  for($y=0;$y -lt 18;$y++) {
    for($x=0;$x -lt 20;$x++) {
      $c=$dst.GetPixel($x,$y)
      if($c.A -lt 32){$v=0}else{$v=[uint16]((($c.R -band 0xF8) -shl 8) -bor (($c.G -band 0xFC) -shl 3) -bor ($c.B -shr 3))}
      if($x -ne 0 -or $y -ne 0){$writer.Write(',')}; $writer.Write(('0x{0:X4}' -f $v))
    }
    $writer.WriteLine()
  }
  $writer.WriteLine('};')
} finally {$writer.Dispose(); $dst.Dispose()}
