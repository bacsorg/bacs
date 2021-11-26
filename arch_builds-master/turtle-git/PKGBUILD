# Maintainer: Aleksey Filippov <sarum9in@gmail.com>
pkgname=turtle-git
pkgver=672.8c68e7a
pkgrel=1
pkgdesc="A C++ Mock_object library based on Boost with a focus on usability, simplicity and flexibility."
arch=('i686' 'x86_64')
url="https://github.com/mat007/turtle"
license=('Boost')
makedepends=('git')
provides=('turtle')
conflicts=('turtle')
source=("$pkgname::git+https://github.com/mat007/turtle.git")
md5sums=('SKIP')

pkgver() {
  cd "$srcdir/$pkgname"
  echo $(git rev-list --count HEAD).$(git rev-parse --short HEAD)
}

package() {
    cd "$srcdir/$pkgname"
    install -dm755 "$pkgdir/usr/include/turtle"
    install -dm755 "$pkgdir/usr/include/turtle/detail"
    install -m644 include/turtle/*.hpp "$pkgdir/usr/include/turtle"
    install -m644 include/turtle/detail/* "$pkgdir/usr/include/turtle/detail"
}
