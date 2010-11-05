# Contributor: Ni@m <niam[dot]niam[at]gmail[dot]com>

pkgname=wset
pkgver=0.0.2
pkgrel=1
pkgdesc="wset is a tool which allows you to compose wallpapers for X"
url="http://github.com/niamster/wset"
license="GPL"
depends=('imlib2' 'xorg-server' 'pygtk')
arch=('i686' 'x86_64')
source=(http://github.com/niamster/wset/tarball/master/$pkgname-$pkgver.tar.gz)
noextract=($pkgname-$pkgver.tar.gz)
md5sums=('e8a7bffef737cc609669aca69cc3c015')

build() {
  mkdir $startdir/src/$pkgname-$pkgver
  tar --strip-components=1 -C $startdir/src/$pkgname-$pkgver -zxf $startdir/src/$pkgname-$pkgver.tar.gz
  cd $startdir/src/$pkgname-$pkgver
  python setup.py build
  python setup.py install --prefix /usr/ --root $startdir/pkg/
}
